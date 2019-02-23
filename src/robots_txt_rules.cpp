#include "robots_txt_rules.h"
#include "robots_txt_rules_impl.h"

namespace cpprobotparser
{

RobotsTxtRules::RobotsTxtRules()
    : m_impl(new RobotsTxtRulesImpl)
{
}

RobotsTxtRules::RobotsTxtRules(const std::string& robotsTxtContent)
    : RobotsTxtRules()
{
    parse(robotsTxtContent);
}

RobotsTxtRules::RobotsTxtRules(const RobotsTxtRules& other)
    : m_impl(new RobotsTxtRulesImpl(*other.m_impl))
{
}

RobotsTxtRules::RobotsTxtRules(RobotsTxtRules&& other)
    : m_impl(other.m_impl)
{
    other.m_impl = nullptr;
}

RobotsTxtRules::~RobotsTxtRules()
{
    delete m_impl;
}

RobotsTxtRules&
RobotsTxtRules::operator=(const RobotsTxtRules& other)
{
    if (this == std::addressof(other))
    {
        return *this;
    }

    *m_impl = *other.m_impl;
    return *this;
}

RobotsTxtRules&
RobotsTxtRules::operator=(RobotsTxtRules&& other)
{
    if (this == std::addressof(other))
    {
        return *this;
    }

    delete m_impl;
    m_impl = other.m_impl;
    other.m_impl = nullptr;

    return *this;
}

void
RobotsTxtRules::parse(const std::string& robotsTxtContent)
{
    m_impl->parse(robotsTxtContent);
}

bool
RobotsTxtRules::isUrlAllowed(const std::string& url, WellKnownUserAgent userAgent) const
{
    return m_impl->isUrlAllowed(url, userAgent);
}

bool
RobotsTxtRules::isUrlAllowed(const std::string& url, const std::string& userAgent) const
{
    return m_impl->isUrlAllowed(url, userAgent);
}

double
RobotsTxtRules::crawlDelay(WellKnownUserAgent userAgent) const
{
    return m_impl->crawlDelay(userAgent);
}

double
RobotsTxtRules::crawlDelay(const std::string& userAgent) const
{
    return m_impl->crawlDelay(userAgent);
}

std::vector<std::string>
RobotsTxtRules::cleanParam(WellKnownUserAgent userAgent) const
{
    return m_impl->cleanParam(userAgent);
}

std::vector<std::string>
RobotsTxtRules::cleanParam(const std::string& userAgent) const
{
    return m_impl->cleanParam(userAgent);
}

}