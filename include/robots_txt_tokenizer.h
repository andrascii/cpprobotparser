#pragma once

#include "url.h"

namespace cpprobotparser
{

enum class UserAgentType;

enum class RobotsTxtToken
{
    TokenUserAgent,
    TokenAllow,
    TokenDisallow,
    TokenSitemap,
    TokenHost,
    TokenCrawlDelay,
    TokenCleanParam,
    TokenCommentary,
    TokenStringDelimeter,
    TokenUnknown
};

class RobotsTxtTokenizer final
{
public:
    struct RobotsTxtTokenVauePair
    {
        RobotsTxtToken token;
        std::string value;
    };

    RobotsTxtTokenizer();

    bool isValid() const noexcept;
    void tokenize(const std::string& robotsTxtContent);

    bool hasUserAgentRecord(UserAgentType userAgentType) const;

    std::vector<std::string> tokenValues(UserAgentType userAgentType, RobotsTxtToken token) const;
    const Url& sitemap() const;
    const Url& originalHostMirror() const;

    QList<RobotsTxtTokenVauePair> allowAndDisallowTokens(UserAgentType userAgentType) const;

private:
    StringHelpers::StringList removeCommentaries(const StringHelpers::StringList& strings);
    std::pair<std::string, std::string> tokenizeRow(const std::string& row) const;

private:
    using Tokens = std::multimap<RobotsTxtToken, std::string>;

    Url m_sitemapUrl;
    Url m_originalHostMirrorUrl;
    std::map<UserAgentType, Tokens> m_userAgentTokens;
    bool m_validRobotsTxt;
};

}