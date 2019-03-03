#pragma once

#include "pimpl.h"
#include "export_macro.h"
#include "well_known_user_agent.h"

namespace cpprobotparser
{

class CPPROBOTPARSER_EXPORT RobotsTxtRules final
{
public:
    RobotsTxtRules();
    RobotsTxtRules(const RobotsTxtRules& other);
    RobotsTxtRules(RobotsTxtRules&& other);
    RobotsTxtRules(const std::string& robotsTxtContent);
    ~RobotsTxtRules();

    RobotsTxtRules& operator=(const RobotsTxtRules& other);
    RobotsTxtRules& operator=(RobotsTxtRules&& other);

    //! Parses the robots.txt content
    void parse(const std::string& robotsTxtContent);

    //! Returns true if passed URL is allowed to crawl for the specified user agent
    //! Note: if you test some URL for example for GoogleBot user agent but robots.txt content
    //! does not contain any rules for Google then it will analyze rules for all robots (rules under this user agent: *)
    bool isUrlAllowed(const std::string& url, WellKnownUserAgent userAgent) const;
    bool isUrlAllowed(const std::string& url, const std::string& userAgent) const;

    //! Returns the seconds to delay between requests for the specified user agent
    double crawlDelay(WellKnownUserAgent userAgent) const;
    double crawlDelay(const std::string& userAgent) const;

    //! Returns the set of Clean-param tokens for the specified user agent
    std::vector<std::string> cleanParam(WellKnownUserAgent userAgent) const;
    std::vector<std::string> cleanParam(const std::string& userAgent) const;

    //! returns true if passed user agent is found in the robots.txt, otherwise returns false
    bool hasRulesFor(WellKnownUserAgent userAgent) const;
    bool hasRulesFor(const std::string& userAgent) const;

    //! returns the URL to the sitemap if it exists in the robots.txt file
    const std::string& sitemapUrl() const noexcept;

private:
    class RobotsTxtRulesImpl;
    Pimpl<RobotsTxtRulesImpl> m_impl;
};

}