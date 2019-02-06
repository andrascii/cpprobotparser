#include "robots_txt_tokenizer.h"
#include "meta_robots_helpers.h"

namespace cpprobotparser
{

namespace
{

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
    { s_tokenToString.at(RobotsTxtToken::TokenUserAgent)/*"user-agent"*/, RobotsTxtToken::TokenUserAgent },
    { s_tokenToString.at(RobotsTxtToken::TokenAllow)/*"allow"*/, RobotsTxtToken::TokenAllow },
    { s_tokenToString.at(RobotsTxtToken::TokenDisallow)/*"disallow"*/, RobotsTxtToken::TokenDisallow },
    { s_tokenToString.at(RobotsTxtToken::TokenSitemap)/*"sitemap"*/, RobotsTxtToken::TokenSitemap },
    { s_tokenToString.at(RobotsTxtToken::TokenHost)/*"host"*/, RobotsTxtToken::TokenHost },
    { s_tokenToString.at(RobotsTxtToken::TokenCrawlDelay)/*"crawl-delay"*/, RobotsTxtToken::TokenCrawlDelay },
    { s_tokenToString.at(RobotsTxtToken::TokenCleanParam)/*"clean-param"*/, RobotsTxtToken::TokenCleanParam },
    { s_tokenToString.at(RobotsTxtToken::TokenCommentary)/*"#"*/, RobotsTxtToken::TokenCommentary },
    { s_tokenToString.at(RobotsTxtToken::TokenStringDelimeter)/*":"*/, RobotsTxtToken::TokenStringDelimeter }
};

} // namespace

RobotsTxtTokenizer::RobotsTxtTokenizer()
    : m_validRobotsTxt(false)
{
}

bool RobotsTxtTokenizer::isValid() const noexcept
{
    return m_validRobotsTxt;
}

void RobotsTxtTokenizer::tokenize(const std::string& robotsTxtContent)
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
            //
            // First token must be a user-agent or sitemap or host
            //
            m_validRobotsTxt = false;
            return;
        }

        if (isUserAgentToken)
        {
            userAgentType = MetaRobotsHelpers::userAgent(tokenValue);
        }
        else
        {
            if (userAgentType == WellKnownUserAgent::Unknown)
            {
                continue;
            }

            RobotsTxtToken tokenEnumerator = RobotsTxtToken::TokenUnknown;

            try
            {
                tokenEnumerator = s_stringToToken.at(token);
            }
            catch (const std::out_of_range&)
            {
            }

            Tokens& tokens = m_userAgentTokens[userAgentType];
            tokens.insert(std::make_pair(tokenEnumerator, tokenValue));

            if (tokenEnumerator == RobotsTxtToken::TokenSitemap)
            {
                if (!m_sitemapUrl.empty())
                {
                    //WARNLOG << "Seems that robots.txt file has several sitemaps";
                    continue;
                }

                m_sitemapUrl = tokenValue;
            }
        }
    }

    m_validRobotsTxt = true;
}

std::vector<std::string>
RobotsTxtTokenizer::tokenValues(WellKnownUserAgent userAgentType, RobotsTxtToken token) const
{
    std::vector<std::string> result;

    try
    {
        const Tokens& tokens = m_userAgentTokens.at(userAgentType);
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
RobotsTxtTokenizer::sitemapUrl() const
{
    return m_sitemapUrl;
}

const std::string&
RobotsTxtTokenizer::originalHostMirrorUrl() const
{
    return m_originalHostMirrorUrl;
}

StringHelpers::StringList
RobotsTxtTokenizer::removeCommentaries(const StringHelpers::StringList& strings)
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
RobotsTxtTokenizer::tokenizeRow(const std::string& row) const
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

bool RobotsTxtTokenizer::hasUserAgentRecord(WellKnownUserAgent userAgentType) const
{
    return m_userAgentTokens.find(userAgentType) != m_userAgentTokens.end();
}

std::vector<RobotsTxtTokenizer::RobotsTxtTokenValuePair>
RobotsTxtTokenizer::allowAndDisallowTokens(WellKnownUserAgent userAgentType) const
{
    std::vector<RobotsTxtTokenValuePair> result;
    const std::vector<std::string> allowTokens = tokenValues(userAgentType, RobotsTxtToken::TokenAllow);
    const std::vector<std::string> disallowTokens = tokenValues(userAgentType, RobotsTxtToken::TokenDisallow);

    for (const std::string& allowToken : allowTokens)
    {
        result.push_back({ RobotsTxtToken::TokenAllow, allowToken });
    }

    for (const std::string& disallowToken : disallowTokens)
    {
        result.push_back({ RobotsTxtToken::TokenDisallow, disallowToken });
    }

    auto sortFunc = [](const RobotsTxtTokenValuePair& first, const RobotsTxtTokenValuePair& second)
    {
        if (first.value.size() != second.value.size())
        {
            return first.value.size() < second.value.size();
        }

        return first.token == RobotsTxtToken::TokenAllow && second.token == RobotsTxtToken::TokenDisallow;
    };

    std::sort(result.begin(), result.end(), sortFunc);
    return result;
}

}
