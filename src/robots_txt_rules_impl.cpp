#include "robots_txt_token.h"
#include "robots_txt_rules_impl.h"
#include "meta_robots_helpers.h"
#include "string_helpers.h"
#include <url.hpp>

namespace cpprobotparser
{

void
RobotsTxtRulesImpl::parse(const std::string& robotsTxtContent)
{
    m_tokenizer.tokenize(robotsTxtContent);
}

bool
RobotsTxtRulesImpl::isUrlAllowed(const std::string& url, WellKnownUserAgent userAgent) const
{
    return isUrlAllowed(url, MetaRobotsHelpers::userAgentString(userAgent));
}

bool
RobotsTxtRulesImpl::isUrlAllowed(const std::string& url, const std::string& userAgent) const
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

    std::vector<TokenValue> tokens;

    {
        std::vector<std::string> allowTokens = m_tokenizer.tokenValues(userAgent, RobotsTxtToken::TokenAllow);
        std::vector<std::string> disallowTokens = m_tokenizer.tokenValues(userAgent, RobotsTxtToken::TokenDisallow);

        for (const std::string& disallowTokenValue : allowTokens)
        {
            tokens.emplace_back(RobotsTxtToken::TokenAllow, disallowTokenValue);
        }
        for (const std::string& disallowTokenValue : disallowTokens)
        {
            tokens.emplace_back(RobotsTxtToken::TokenDisallow, disallowTokenValue);
        }
    }

    bool allowed = true;

    for(const TokenValue& token : tokens)
    {
        if (patternMatched(token.value, urlPath))
        {
            allowed = token.type == RobotsTxtToken::TokenAllow ? true : false;
        }
    }

    return allowed;
}

double
RobotsTxtRulesImpl::crawlDelay(WellKnownUserAgent userAgent) const
{
    return crawlDelay(MetaRobotsHelpers::userAgentString(userAgent));
}

double
RobotsTxtRulesImpl::crawlDelay(const std::string& userAgent) const
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

std::vector<std::string>
RobotsTxtRulesImpl::cleanParam(WellKnownUserAgent userAgent) const
{
    return m_tokenizer.tokenValues(userAgent, RobotsTxtToken::TokenCleanParam);
}

std::vector<std::string>
RobotsTxtRulesImpl::cleanParam(const std::string& userAgent) const
{
    return m_tokenizer.tokenValues(userAgent, RobotsTxtToken::TokenCleanParam);
}

bool RobotsTxtRulesImpl::patternMatched(const std::string& pattern, const std::string& value) const
{
    const std::string cannonicalPattern = StringHelpers::toLower(pattern);
    const std::string cannonicalValue = StringHelpers::toLower(value);

    const std::string::size_type dollarIndex = cannonicalPattern.find("$");
    const bool patternContainsStar = cannonicalPattern.find("*") != std::string::npos;
    const bool patternContainsDollar = dollarIndex != std::string::npos;

    if (!patternContainsStar && !patternContainsDollar)
    {
        return StringHelpers::startsWith(cannonicalValue, cannonicalPattern);
    }

    if (patternContainsDollar && dollarIndex != cannonicalPattern.size() - 1)
    {
        // bad pattern
        return false;
    }

    StringHelpers::StringList parts = StringHelpers::splitString(
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
                StringHelpers::endsWith(cannonicalValue, parts[i]) :
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

    return matched;
}

}