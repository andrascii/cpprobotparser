#include "robots_txt_tokenizer.h"
#include "meta_robots_helpers.h"

namespace cpprobotparser
{

namespace
{

const std::map<RobotsTxtToken, std::string_view> s_tokenToString =
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

const std::map<std::string_view, RobotsTxtToken> s_stringToToken =
{
    { "user-agent", RobotsTxtToken::TokenUserAgent },
    { "allow", RobotsTxtToken::TokenAllow },
    { "disallow", RobotsTxtToken::TokenDisallow },
    { "sitemap", RobotsTxtToken::TokenSitemap },
    { "host", RobotsTxtToken::TokenHost },
    { "crawl-delay", RobotsTxtToken::TokenCrawlDelay },
    { "clean-param", RobotsTxtToken::TokenCleanParam },
    { "#", RobotsTxtToken::TokenCommentary },
    { ":", RobotsTxtToken::TokenStringDelimeter }
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
    static std::regex s_endOfLine("(\\n|\\r|\\r\\n|\\n\\r)");

    StringHelpers::StringList rows = removeCommentaries(
        StringHelpers::splitString(robotsTxtContent, s_endOfLine, StringHelpers::SkipEmptyParts));

    UserAgentType userAgentType = UserAgentType::AnyBot;

    for (int i = 0; i < rows.size(); ++i)
    {
        auto[token, tokenValue] = tokenizeRow(rows[i]);

        const bool isUserAgentToken = token == s_tokenToString[RobotsTxtToken::TokenUserAgent];
        const bool isSitemapToken = token == s_tokenToString[RobotsTxtToken::TokenSitemap];
        const bool isHostToken = token == s_tokenToString[RobotsTxtToken::TokenHost];

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
            if (userAgentType == UserAgentType::Unknown)
            {
                continue;
            }

            RobotsTxtToken tokenEnumerator = s_stringToToken.value(token, RobotsTxtToken::TokenUnknown);
            Tokens& tokens = m_userAgentTokens[userAgentType];
            tokens.insert(tokenEnumerator, tokenValue);

            if (tokenEnumerator == RobotsTxtToken::TokenSitemap)
            {
                if (m_sitemapUrl.isValid())
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

QList<QString> RobotsTxtTokenizer::tokenValues(UserAgentType userAgentType, RobotsTxtToken token) const
{
    return m_userAgentTokens.contains(userAgentType) ?
        m_userAgentTokens.value(userAgentType).values(token) : QList<QString>();
}

const Url& RobotsTxtTokenizer::sitemap() const
{
    return m_sitemapUrl;
}

const Url& RobotsTxtTokenizer::originalHostMirror() const
{
    return m_originalHostMirrorUrl;
}

StringHelpers::StringList RobotsTxtTokenizer::removeCommentaries(const StringHelpers::StringList& strings)
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

std::pair<std::string, std::string> RobotsTxtTokenizer::tokenizeRow(const QString& row) const
{
	const size_t tokenPartStringDelimeterPosition =
		row.find(s_tokenToString[RobotsTxtToken::TokenStringDelimeter]);

	if (tokenPartStringDelimeterPosition == -1)
	{
		// invalid row
		return std::make_pair(std::string(), std::string());
	}

	const std::string token = row.left(tokenPartStringDelimeterPosition).trimmed().toLower();
	const std::string tokenValue = row.right(row.size() - tokenPartStringDelimeterPosition - 1).trimmed().toLower();

	return std::make_pair(token, tokenValue);
}

bool RobotsTxtTokenizer::hasUserAgentRecord(UserAgentType userAgentType) const
{
	return m_userAgentTokens.contains(userAgentType);
}

QList<RobotsTxtTokenizer::RobotsTxtTokenVauePair> RobotsTxtTokenizer::allowAndDisallowTokens(UserAgentType userAgentType) const
{
    QVector<RobotsTxtTokenVauePair> result;
    const QList<QString> allowTokens = tokenValues(userAgentType, RobotsTxtToken::TokenAllow);
    const QList<QString> disallowTokens = tokenValues(userAgentType, RobotsTxtToken::TokenDisallow);

    foreach(const QString& allowToken, allowTokens)
    {
        result.push_back({ RobotsTxtToken::TokenAllow, allowToken });
    }

    foreach(const QString& disallowToken, disallowTokens)
    {
        result.push_back({ RobotsTxtToken::TokenDisallow, disallowToken });
    }

    auto sortFunc = [](const RobotsTxtTokenVauePair& first, const RobotsTxtTokenVauePair& second)
    {
        if (first.value.size() != second.value.size())
        {
            return first.value.size() < second.value.size();
        }

        return first.token == RobotsTxtToken::TokenAllow && second.token == RobotsTxtToken::TokenDisallow;
    };

    std::sort(result.begin(), result.end(), sortFunc);
    return result.toList();
}

}
