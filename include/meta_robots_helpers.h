#pragma once

#include "export_macro.h"

namespace cpprobotparser
{

enum class WellKnownUserAgent;
class RobotsTxtTokenizer;

class CPPROBOTPARSER_EXPORT MetaRobotsHelpers
{
public:
    static WellKnownUserAgent userAgent(const std::string& userAgentStr);
    static std::string userAgentString(WellKnownUserAgent wellKnownUserAgent);
    static std::vector<WellKnownUserAgent> wellKnownUserAgents();
};

}