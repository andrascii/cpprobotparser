#pragma once

#include "robots_txt_rules.h"
#include "well_known_user_agent.h"

namespace cpprobotparser
{

enum class WellKnownUserAgent;

class RobotsTxtRules final
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
    bool isUrlAllowed(const std::string& url, WellKnownUserAgent userAgent) const;
    bool isUrlAllowed(const std::string& url, const std::string& userAgent) const;

    //! Returns the seconds to delay between requests for the specified user agent
    double crawlDelay(WellKnownUserAgent userAgent) const;
    double crawlDelay(const std::string& userAgent) const;

    //! Returns the set of Clean-param tokens for the specified user agent
    std::vector<std::string> cleanParam(WellKnownUserAgent userAgent) const;
    std::vector<std::string> cleanParam(const std::string& userAgent) const;

private:
    class RobotsTxtRulesImpl;
    std::unique_ptr<RobotsTxtRulesImpl> m_impl;
};

}