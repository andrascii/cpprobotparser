﻿#include "robots_txt_rules.h"
#include "robots_txt_token.h"
#include "robots_txt_tokenizer.h"
#include "meta_robots_helpers.h"
#include "string_helpers.h"
#include <url.hpp>

namespace cpprobotparser
{

class RobotsTxtRules::RobotsTxtRulesImpl final
{
private:
    struct TokenValue
    {
        TokenValue(RobotsTxtToken tokenType, const std::string& tokenValue)
            : type(tokenType)
            , value(tokenValue)
        {
        }

        RobotsTxtToken type;
        std::string value;
    };

public:
    void parse(const std::string& robotsTxtContent)
    {
        m_tokenizer.tokenize(robotsTxtContent);
    }

    bool isUrlAllowed(const std::string& url, WellKnownUserAgent userAgent) const
    {
        return isUrlAllowed(url, MetaRobotsHelpers::userAgentString(userAgent));
    }

    bool isUrlAllowed(const std::string& url, const std::string& userAgent) const
    {
        if (!m_tokenizer.isValid())
        {
            return true;
        }

        const auto makeQueryString = [](const Url::Query& query)
        {
            std::string resultQuery;

            for (const Url::KeyVal& keyVal : query)
            {
                resultQuery += keyVal.key() + "=" + keyVal.val();
            }

            return resultQuery;
        };

        const Url cleanedUrl = url;
        const Url::Query query = cleanedUrl.query();
        const std::string urlPath = cleanedUrl.path() + (query.empty() ? std::string() : std::string("?") + makeQueryString(query));

        std::vector<TokenValue> tokens = allowAndDisallowTokensFor(userAgent);

        if (tokens.empty())
        {
            tokens = allowAndDisallowTokensFor(MetaRobotsHelpers::userAgentString(WellKnownUserAgent::AllRobots));
        }

        std::vector<std::pair<int, bool>> allowDisallowResults;

        for (const TokenValue& token : tokens)
        {
            const std::pair<bool, int> result = patternMatched(token.value, urlPath);

            if (result.first)
            {
                const bool isAllowed = token.type == RobotsTxtToken::TokenAllow ? true : false;
                allowDisallowResults.push_back(std::make_pair(result.second, isAllowed));
            }
        }

        std::sort(allowDisallowResults.begin(), allowDisallowResults.end(), [](const auto& lhs, const auto& rhs)
            {
                return lhs.first > rhs.first;
            });

        // if URL is not matched to any pattern then we treat this as an allowed URL
        return allowDisallowResults.empty() ? true : allowDisallowResults.front().second;
    }

    double crawlDelay(WellKnownUserAgent userAgent) const
    {
        return crawlDelay(MetaRobotsHelpers::userAgentString(userAgent));
    }

    double crawlDelay(const std::string& userAgent) const
    {
        try
        {
            const std::vector<std::string> tokenValues =
                m_tokenizer.tokenValues(userAgent, RobotsTxtToken::TokenCrawlDelay);

            if (tokenValues.empty())
            {
                throw std::runtime_error("There is no crawl delay token for specified user agent");
            }

            return std::stod(tokenValues.front());
        }
        catch (const std::invalid_argument&)
        {
            throw std::runtime_error("Invalid crawl delay format");
        }
        catch (const std::out_of_range&)
        {
            throw std::runtime_error("The resulting double number is out of range");
        }
    }

    std::vector<std::string> cleanParam(WellKnownUserAgent userAgent) const
    {
        return m_tokenizer.tokenValues(userAgent, RobotsTxtToken::TokenCleanParam);
    }

    std::vector<std::string> cleanParam(const std::string& userAgent) const
    {
        return m_tokenizer.tokenValues(userAgent, RobotsTxtToken::TokenCleanParam);
    }

    bool hasRulesFor(WellKnownUserAgent userAgent) const
    {
        return m_tokenizer.hasUserAgentRecord(userAgent);
    }

    bool hasRulesFor(const std::string& userAgent) const
    {
        return m_tokenizer.hasUserAgentRecord(userAgent);
    }

    const std::string& sitemapUrl() const noexcept
    {
        return m_tokenizer.sitemapUrl();
    }

private:
    // returns true if the value matches the pattern and folder nesting level
    std::pair<bool, int> patternMatched(const std::string& pattern, const std::string& value) const
    {
        const std::string cannonicalPattern = StringHelpers::toLower(pattern);
        const std::string cannonicalValue = StringHelpers::toLower(value);

        const std::string::size_type dollarIndex = cannonicalPattern.find("$");
        const bool patternContainsStar = cannonicalPattern.find("*") != std::string::npos;
        const bool patternContainsDollar = dollarIndex != std::string::npos;

        const int nestingPatternLevel = static_cast<int>(StringHelpers::split(
            cannonicalPattern,
            "/",
            StringHelpers::SkipEmptyParts,
            StringHelpers::CaseSensitive).size());

        if (!patternContainsStar && !patternContainsDollar)
        {
            return std::make_pair(StringHelpers::startsWith(cannonicalValue, cannonicalPattern), nestingPatternLevel);
        }

        if (patternContainsDollar && dollarIndex != cannonicalPattern.size() - 1)
        {
            // bad pattern
            return std::make_pair(false, nestingPatternLevel);
        }

        StringHelpers::StringList parts = StringHelpers::split(
            cannonicalPattern, "*", StringHelpers::SkipEmptyParts, StringHelpers::CaseSensitive);

        std::string::size_type index = 0;
        bool matched = true;

        for (int i = 0; i < parts.size(); i++)
        {
            const bool lastPart = i == parts.size() - 1;
            bool strongMatch = false;

            if (lastPart && StringHelpers::endsWith(parts[i], "$"))
            {
                strongMatch = true;
                assert(!strongMatch || lastPart);
                parts[i] = parts[i].substr(0, parts[i].size() - 1);
            }

            if (i == 0 || StringHelpers::startsWith(cannonicalPattern, "*"))
            {
                matched = strongMatch ?
                    StringHelpers::endsWith(cannonicalValue, parts[i], StringHelpers::CaseInsensitive) :
                    cannonicalValue.find(parts[i]) != std::string::npos;

                if (!matched)
                {
                    break;
                }

                if (!strongMatch)
                {
                    // can return std::string::npos
                    index = cannonicalValue.find(parts[i]) + parts[i].size();
                }

                continue;
            }

            const std::string::size_type matchedIndex = cannonicalValue.find(parts[i], index);
            matched = strongMatch ? matchedIndex + parts[i].size() == cannonicalPattern.size() - 1 : matchedIndex != -1;

            if (!matched)
            {
                break;
            }

            index = matchedIndex + parts[i].size();
        }

        return std::make_pair(matched, nestingPatternLevel);
    }

    std::vector<TokenValue> allowAndDisallowTokensFor(const std::string& userAgent) const
    {
        std::vector<std::string> allowTokens = m_tokenizer.tokenValues(userAgent, RobotsTxtToken::TokenAllow);
        std::vector<std::string> disallowTokens = m_tokenizer.tokenValues(userAgent, RobotsTxtToken::TokenDisallow);

        std::vector<TokenValue> tokens;

        for (const std::string& disallowTokenValue : allowTokens)
        {
            tokens.emplace_back(RobotsTxtToken::TokenAllow, disallowTokenValue);
        }
        for (const std::string& disallowTokenValue : disallowTokens)
        {
            tokens.emplace_back(RobotsTxtToken::TokenDisallow, disallowTokenValue);
        }

        return tokens;
    }

private:
    RobotsTxtTokenizer m_tokenizer;
};

//////////////////////////////////////////////////////////////////////////

RobotsTxtRules::RobotsTxtRules(const std::string& robotsTxtContent)
    : RobotsTxtRules()
{
    parse(robotsTxtContent);
}

RobotsTxtRules::RobotsTxtRules() = default;
RobotsTxtRules::RobotsTxtRules(const RobotsTxtRules& other) = default;
RobotsTxtRules::RobotsTxtRules(RobotsTxtRules&& other) = default;
RobotsTxtRules::~RobotsTxtRules() = default;
RobotsTxtRules& RobotsTxtRules::operator=(const RobotsTxtRules& other) = default;
RobotsTxtRules& RobotsTxtRules::operator=(RobotsTxtRules&& other) = default;

void RobotsTxtRules::parse(const std::string& robotsTxtContent)
{
    m_impl->parse(robotsTxtContent);
}

bool RobotsTxtRules::isUrlAllowed(const std::string& url, WellKnownUserAgent userAgent) const
{
    return m_impl->isUrlAllowed(url, userAgent);
}

bool RobotsTxtRules::isUrlAllowed(const std::string& url, const std::string& userAgent) const
{
    return m_impl->isUrlAllowed(url, userAgent);
}

double RobotsTxtRules::crawlDelay(WellKnownUserAgent userAgent) const
{
    return m_impl->crawlDelay(userAgent);
}

double RobotsTxtRules::crawlDelay(const std::string& userAgent) const
{
    return m_impl->crawlDelay(userAgent);
}

std::vector<std::string> RobotsTxtRules::cleanParam(WellKnownUserAgent userAgent) const
{
    return m_impl->cleanParam(userAgent);
}

std::vector<std::string> RobotsTxtRules::cleanParam(const std::string& userAgent) const
{
    return m_impl->cleanParam(userAgent);
}

bool RobotsTxtRules::hasRulesFor(WellKnownUserAgent userAgent) const
{
    return m_impl->hasRulesFor(userAgent);
}

bool RobotsTxtRules::hasRulesFor(const std::string& userAgent) const
{
    return m_impl->hasRulesFor(userAgent);
}

const std::string& RobotsTxtRules::sitemapUrl() const noexcept
{
    return m_impl->sitemapUrl();
}

}