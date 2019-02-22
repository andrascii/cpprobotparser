#pragma once

#include "string_helpers.h"

namespace cpprobotparser
{

enum class WellKnownUserAgent;
enum class RobotsTxtToken;

class RobotsTxtTokenizerImpl final
{
public:
    RobotsTxtTokenizerImpl();

    bool isValid() const noexcept;
    void tokenize(const std::string& robotsTxtContent);

    bool hasUserAgentRecord(WellKnownUserAgent userAgentType) const;
    bool hasUserAgentRecord(const std::string& userAgent) const;

    std::vector<std::string> tokenValues(WellKnownUserAgent userAgentType, RobotsTxtToken token) const;
    std::vector<std::string> tokenValues(const std::string& userAgent, RobotsTxtToken token) const;

    const std::string& sitemapUrl() const;
    const std::string& originalHostMirrorUrl() const;

private:
    StringHelpers::StringList removeCommentaries(const StringHelpers::StringList& strings);
    std::pair<std::string, std::string> tokenizeRow(const std::string& row) const;

private:
    using Tokens = std::multimap<RobotsTxtToken, std::string>;

    std::string m_sitemapUrl;
    std::string m_originalHostMirrorUrl;
    std::map<std::string, Tokens> m_userAgentTokens;
    bool m_validRobotsTxt;
};

}