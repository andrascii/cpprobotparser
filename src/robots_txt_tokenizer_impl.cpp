#include "robots_txt_tokenizer_impl.h"
#include "robots_txt_token.h"
#include "meta_robots_helpers.h"

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

RobotsTxtTokenizerImpl::RobotsTxtTokenizerImpl()
    : m_validRobotsTxt(false)
{
}

bool RobotsTxtTokenizerImpl::isValid() const noexcept
{
    return m_validRobotsTxt;
}

void RobotsTxtTokenizerImpl::tokenize(const std::string& robotsTxtContent)
{
    StringHelpers::StringList rows = removeCommentaries(
        StringHelpers::splitString(robotsTxtContent, [](char ch) { return ch == '\n'; }, StringHelpers::SkipEmptyParts));

    WellKnownUserAgent userAgentType = WellKnownUserAgent::AnyBot;

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
                if (!m_sitemapUrl.empty())
                {
                    continue;
                }

                m_sitemapUrl = tokenValue;
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

std::vector<std::string>
RobotsTxtTokenizerImpl::tokenValues(WellKnownUserAgent userAgentType, RobotsTxtToken token) const
{
    return tokenValues(MetaRobotsHelpers::userAgentString(userAgentType), token);
}

std::vector<std::string>
RobotsTxtTokenizerImpl::tokenValues(const std::string& userAgent, RobotsTxtToken token) const
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

const std::string&
RobotsTxtTokenizerImpl::sitemapUrl() const
{
    return m_sitemapUrl;
}

const std::string&
RobotsTxtTokenizerImpl::originalHostMirrorUrl() const
{
    return m_originalHostMirrorUrl;
}

StringHelpers::StringList
RobotsTxtTokenizerImpl::removeCommentaries(const StringHelpers::StringList& strings)
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

std::pair<std::string, std::string>
RobotsTxtTokenizerImpl::tokenizeRow(const std::string& row) const
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

bool RobotsTxtTokenizerImpl::hasUserAgentRecord(WellKnownUserAgent userAgentType) const
{
    return m_userAgentTokens.find(MetaRobotsHelpers::userAgentString(userAgentType)) != m_userAgentTokens.end();
}

bool RobotsTxtTokenizerImpl::hasUserAgentRecord(const std::string& userAgent) const
{
    return m_userAgentTokens.find(userAgent) != m_userAgentTokens.end();
}

}