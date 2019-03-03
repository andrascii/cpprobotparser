#pragma once

#include "pimpl.h"
#include "export_macro.h"
#include "robots_txt_rules.h"
#include "robots_txt_token.h"
#include "well_known_user_agent.h"

namespace cpprobotparser
{

//! Non thread-safe
//! This tokenizer is used to only parse robots.txt file
//! and provide you rules for each user agent occurred in the file.
class CPPROBOTPARSER_EXPORT RobotsTxtTokenizer final
{
public:
    RobotsTxtTokenizer();
    RobotsTxtTokenizer(const std::string& robotsTxtContent);
    RobotsTxtTokenizer(const RobotsTxtTokenizer& other);
    RobotsTxtTokenizer(RobotsTxtTokenizer&& other);
    ~RobotsTxtTokenizer();

    RobotsTxtTokenizer& operator=(const RobotsTxtTokenizer& other);
    RobotsTxtTokenizer& operator=(RobotsTxtTokenizer&& other);

    //! returns true if no error occurred, otherwise returns false
    bool isValid() const noexcept;

    //! parse the passed robots.txt content
    void tokenize(const std::string& robotsTxtContent);

    //! returns true if passed user agent is found in the robots.txt, otherwise returns false
    bool hasUserAgentRecord(WellKnownUserAgent userAgentType) const;
    bool hasUserAgentRecord(const std::string& userAgent) const;

    //! returns all token values for specified user agent and specified token
    std::vector<std::string> tokenValues(WellKnownUserAgent userAgentType, RobotsTxtToken token) const;
    std::vector<std::string> tokenValues(const std::string& userAgent, RobotsTxtToken token) const;

    //! returns the URL to the sitemap if it exists in the robots.txt file
    const std::string& sitemapUrl() const noexcept;

    //! returns the URL to the original host mirror if it exists in the robots.txt file
    const std::string& originalHostMirrorUrl() const noexcept;

private:
    class RobotsTxtTokenizerImpl;
    Pimpl<RobotsTxtTokenizerImpl> m_impl;
};

}