#pragma once

#include "robots_txt_tokenizer.h"

namespace cpprobotparser
{

enum class WellKnownUserAgent;

class RobotsTxtRulesImpl final
{
public:
    void parse(const std::string& robotsTxtContent);

    bool isUrlAllowed(const std::string& url, WellKnownUserAgent userAgent) const;
    bool isUrlAllowed(const std::string& url, const std::string& userAgent) const;

    double crawlDelay(WellKnownUserAgent userAgent) const;
    double crawlDelay(const std::string& userAgent) const;

    std::vector<std::string> cleanParam(WellKnownUserAgent userAgent) const;
    std::vector<std::string> cleanParam(const std::string& userAgent) const;

private:
    bool patternMatched(const std::string& pattern, const std::string& value) const;

private:
    RobotsTxtTokenizer m_tokenizer;
};


}