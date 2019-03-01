#include "robots_txt_tokenizer.h"
#include "robots_txt_token.h"
#include "string_helpers.h"
#include "meta_robots_helpers.h"
#include "well_known_user_agent.h"

namespace
{

using namespace cpprobotparser;

const std::map<RobotsTxtToken, std::string> s_tokenToString =
{
    { RobotsTxtToken::TokenUserAgent, "user-agent" },
    { RobotsTxtToken::TokenAllow, "allow" },
    { RobotsTxtToken::TokenDisallow, "disallow" },
    { RobotsTxtToken::TokenSitemap, "sitemap" },
    { RobotsTxtToken::TokenHost, "host" },
    { RobotsTxtToken::TokenCrawlDelay, "crawl-delay" },
    { RobotsTxtToken::TokenCleanParam, "clean-param" },
    { RobotsTxtToken::TokenCommentary, "#" },
    { RobotsTxtToken::TokenStringDelimeter, ":" }
};

const std::map<std::string, RobotsTxtToken> s_stringToToken =
{
    { s_tokenToString.at(RobotsTxtToken::TokenUserAgent), RobotsTxtToken::TokenUserAgent },
    { s_tokenToString.at(RobotsTxtToken::TokenAllow), RobotsTxtToken::TokenAllow },
    { s_tokenToString.at(RobotsTxtToken::TokenDisallow), RobotsTxtToken::TokenDisallow },
    { s_tokenToString.at(RobotsTxtToken::TokenSitemap), RobotsTxtToken::TokenSitemap },
    { s_tokenToString.at(RobotsTxtToken::TokenHost), RobotsTxtToken::TokenHost },
    { s_tokenToString.at(RobotsTxtToken::TokenCrawlDelay), RobotsTxtToken::TokenCrawlDelay },
    { s_tokenToString.at(RobotsTxtToken::TokenCleanParam), RobotsTxtToken::TokenCleanParam },
    { s_tokenToString.at(RobotsTxtToken::TokenCommentary), RobotsTxtToken::TokenCommentary },
    { s_tokenToString.at(RobotsTxtToken::TokenStringDelimeter), RobotsTxtToken::TokenStringDelimeter }
};

} // namespace

namespace cpprobotparser
{

class RobotsTxtTokenizer::RobotsTxtTokenizerImpl final
{
public:
    RobotsTxtTokenizerImpl()
        : m_validRobotsTxt(false)
    {
    }

    bool isValid() const noexcept
    {
        return m_validRobotsTxt;
    }

    void tokenize(const std::string& robotsTxtContent)
    {
        StringHelpers::StringList rows = removeCommentaries(
            StringHelpers::splitString(robotsTxtContent, [](char ch) { return ch == '\n'; }, StringHelpers::SkipEmptyParts));

        WellKnownUserAgent userAgentType = WellKnownUserAgent::AllRobots;

        for (int i = 0; i < rows.size(); ++i)
        {
            std::pair<std::string, std::string> tokenizedRow = tokenizeRow(rows[i]);

            const std::string token = std::move(tokenizedRow.first);
            const std::string tokenValue = std::move(tokenizedRow.second);

            const bool isUserAgentToken = token == s_tokenToString.at(RobotsTxtToken::TokenUserAgent);
            const bool isSitemapToken = token == s_tokenToString.at(RobotsTxtToken::TokenSitemap);
            const bool isHostToken = token == s_tokenToString.at(RobotsTxtToken::TokenHost);

            if (!isUserAgentToken && !isSitemapToken && !isHostToken && i == 0)
            {
                // First token must be a user-agent or sitemap or host
                m_validRobotsTxt = false;
                return;
            }

            if (isUserAgentToken)
            {
                userAgentType = MetaRobotsHelpers::userAgent(tokenValue);
            }
            else
            {
                RobotsTxtToken tokenEnumerator = RobotsTxtToken::TokenUnknown;

                try
                {
                    tokenEnumerator = s_stringToToken.at(token);
                }
                catch (const std::out_of_range&)
                {
                }

                if (tokenEnumerator == RobotsTxtToken::TokenSitemap)
                {
                    m_sitemapUrl = tokenValue;
                    continue;
                }

                if (userAgentType == WellKnownUserAgent::Unknown)
                {
                    continue;
                }

                Tokens& tokens = m_userAgentTokens[MetaRobotsHelpers::userAgentString(userAgentType)];
                tokens.insert(std::make_pair(tokenEnumerator, tokenValue));
            }
        }

        m_validRobotsTxt = true;
    }

    bool hasUserAgentRecord(WellKnownUserAgent userAgentType) const
    {
        return m_userAgentTokens.find(MetaRobotsHelpers::userAgentString(userAgentType)) != m_userAgentTokens.end();
    }

    bool hasUserAgentRecord(const std::string& userAgent) const
    {
        return m_userAgentTokens.find(userAgent) != m_userAgentTokens.end();
    }

    std::vector<std::string> tokenValues(WellKnownUserAgent userAgentType, RobotsTxtToken token) const
    {
        return tokenValues(MetaRobotsHelpers::userAgentString(userAgentType), token);
    }

    std::vector<std::string> tokenValues(const std::string& userAgent, RobotsTxtToken token) const
    {
        std::vector<std::string> result;

        try
        {
            const Tokens& tokens = m_userAgentTokens.at(userAgent);
            const auto rangePair = tokens.equal_range(token);

            std::for_each(rangePair.first, rangePair.second, [&result](const auto& iter)
                {
                    result.push_back(iter.second);
                });
        }
        catch (const std::out_of_range&)
        {
        } // do nothing

        return result;
    }

    const std::string& sitemapUrl() const noexcept
    {
        return m_sitemapUrl;
    }

    const std::string& originalHostMirrorUrl() const noexcept
    {
        return m_originalHostMirrorUrl;
    }

private:
    StringHelpers::StringList removeCommentaries(const StringHelpers::StringList& strings) const
    {
        StringHelpers::StringList resultStrings;

        for (const std::string& string : strings)
        {
            const std::string validatedString = StringHelpers::removeAllFrom(string, "#");

            if (validatedString.empty())
            {
                continue;
            }

            resultStrings.push_back(validatedString);
        }

        return resultStrings;
    }

    std::pair<std::string, std::string> tokenizeRow(const std::string& row) const
    {
        const size_t tokenPartStringDelimeterPosition =
            row.find(s_tokenToString.at(RobotsTxtToken::TokenStringDelimeter));

        if (tokenPartStringDelimeterPosition == std::string::npos)
        {
            // invalid row
            return std::make_pair(std::string(), std::string());
        }

        const std::string token = StringHelpers::trimmed(
            StringHelpers::toLower(row.substr(0, tokenPartStringDelimeterPosition)));

        const std::string tokenValue = StringHelpers::trimmed(
            StringHelpers::toLower(row.substr(tokenPartStringDelimeterPosition + 1, row.size())));

        return std::make_pair(token, tokenValue);
    }

private:
    using Tokens = std::multimap<RobotsTxtToken, std::string>;

    std::string m_sitemapUrl;
    std::string m_originalHostMirrorUrl;
    std::map<std::string, Tokens> m_userAgentTokens;
    bool m_validRobotsTxt;
};

//////////////////////////////////////////////////////////////////////////

RobotsTxtTokenizer::RobotsTxtTokenizer(const std::string& robotsTxtContent)
    : RobotsTxtTokenizer()
{
    tokenize(robotsTxtContent);
}

RobotsTxtTokenizer::RobotsTxtTokenizer() = default;
RobotsTxtTokenizer::RobotsTxtTokenizer(const RobotsTxtTokenizer& other) = default;
RobotsTxtTokenizer::RobotsTxtTokenizer(RobotsTxtTokenizer&& other) = default;
RobotsTxtTokenizer::~RobotsTxtTokenizer() = default;
RobotsTxtTokenizer& RobotsTxtTokenizer::operator=(const RobotsTxtTokenizer& other) = default;
RobotsTxtTokenizer& RobotsTxtTokenizer::operator=(RobotsTxtTokenizer&& other) = default;

bool RobotsTxtTokenizer::isValid() const noexcept
{
    return m_impl->isValid();
}

void RobotsTxtTokenizer::tokenize(const std::string& robotsTxtContent)
{
    m_impl->tokenize(robotsTxtContent);
}

std::vector<std::string> RobotsTxtTokenizer::tokenValues(WellKnownUserAgent userAgentType, RobotsTxtToken token) const
{
    return m_impl->tokenValues(userAgentType, token);
}

std::vector<std::string> RobotsTxtTokenizer::tokenValues(const std::string& userAgent, RobotsTxtToken token) const
{
    return m_impl->tokenValues(userAgent, token);
}

const std::string& RobotsTxtTokenizer::sitemapUrl() const noexcept
{
    return m_impl->sitemapUrl();
}

const std::string& RobotsTxtTokenizer::originalHostMirrorUrl() const noexcept
{
    return m_impl->originalHostMirrorUrl();
}

bool RobotsTxtTokenizer::hasUserAgentRecord(WellKnownUserAgent userAgentType) const
{
    return m_impl->hasUserAgentRecord(userAgentType);
}

bool RobotsTxtTokenizer::hasUserAgentRecord(const std::string& userAgent) const
{
    return m_impl->hasUserAgentRecord(userAgent);
}

}
