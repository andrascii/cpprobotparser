#pragma once

namespace cpprobotparser
{

enum class WellKnownUserAgent;
class RobotsTxtTokenizer;

class MetaRobotsHelpers
{
public:
    static WellKnownUserAgent userAgent(const std::string& userAgentStr);
    static std::string userAgentString(WellKnownUserAgent wellKnownUserAgent);
    static std::vector<WellKnownUserAgent> wellKnownUserAgents();
};

}