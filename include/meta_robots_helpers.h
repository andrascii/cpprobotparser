#pragma once

#include "well_known_user_agent.h"

namespace cpprobotparser
{

class RobotsTxtTokenizer;

class MetaRobotsHelpers
{
public:
    static WellKnownUserAgent userAgent(const std::string& userAgentStr);
    static std::string userAgentString(WellKnownUserAgent wellKnownUserAgent);
    static std::vector<WellKnownUserAgent> wellKnownUserAgents();
};

}