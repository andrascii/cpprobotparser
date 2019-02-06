#pragma once

#include "string_helpers.h"

namespace cpprobotparser
{

enum class WellKnownUserAgent;

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
    struct RobotsTxtTokenValuePair
    {
        RobotsTxtToken token;
        std::string value;
    };

    RobotsTxtTokenizer();

    //! returns true if no error occurred, otherwise returns false
    bool isValid() const noexcept;

    //! parse the passed robots.txt content
    void tokenize(const std::string& robotsTxtContent);

    //! returns true if passed user agent is found in the robots.txt, otherwise returns false
    bool hasUserAgentRecord(WellKnownUserAgent userAgentType) const;

    //! returns all token values for specified user agent and specified token
    std::vector<std::string> tokenValues(WellKnownUserAgent userAgentType, RobotsTxtToken token) const;

    //! returns the URL to the sitemap if it exists in the robots.txt file
    const std::string& sitemapUrl() const;

    //! returns the URL to the original host mirror if it exists in the robots.txt file
    const std::string& originalHostMirrorUrl() const;

    std::vector<RobotsTxtTokenValuePair> allowAndDisallowTokens(WellKnownUserAgent userAgentType) const;

private:
    StringHelpers::StringList removeCommentaries(const StringHelpers::StringList& strings);
    std::pair<std::string, std::string> tokenizeRow(const std::string& row) const;

private:
    using Tokens = std::multimap<RobotsTxtToken, std::string>;

    std::string m_sitemapUrl;
    std::string m_originalHostMirrorUrl;
    std::map<WellKnownUserAgent, Tokens> m_userAgentTokens;
    bool m_validRobotsTxt;
};

}