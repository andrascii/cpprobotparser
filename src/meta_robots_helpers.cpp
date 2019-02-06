#include "meta_robots_helpers.h"
#include "robots_txt_tokenizer.h"

namespace cpprobotparser
{

namespace
{

const std::map<std::string, WellKnownUserAgent> s_userAgent =
{
    { "googlebot", WellKnownUserAgent::GoogleBot },
    { "yandex", WellKnownUserAgent::YandexBot },
    { "mail.ru", WellKnownUserAgent::MailRuBot },
    { "msnbot", WellKnownUserAgent::MsnBot },
    { "slurp", WellKnownUserAgent::YahooBot },
    { "*", WellKnownUserAgent::AnyBot }
};

const std::map<WellKnownUserAgent, std::string> s_userAgentByType =
{
    { WellKnownUserAgent::GoogleBot, "googlebot" },
    { WellKnownUserAgent::YandexBot, "yandex" },
    { WellKnownUserAgent::MailRuBot, "mail.ru" },
    { WellKnownUserAgent::MsnBot, "msnbot" },
    { WellKnownUserAgent::YahooBot, "slurp" },
    { WellKnownUserAgent::AnyBot, "*" }
};

}

WellKnownUserAgent MetaRobotsHelpers::userAgent(const std::string& userAgentStr)
{
    const std::string userAgentValidated = StringHelpers::trimmed(StringHelpers::toLower(userAgentStr));
    const std::string fixedUserAgentStr = userAgentValidated == "robots" ? std::string("*") : userAgentValidated;

    const auto userAgentIterator = s_userAgent.find(fixedUserAgentStr);

    if (userAgentIterator == s_userAgent.end())
    {
        return WellKnownUserAgent::Unknown;
    }

    return userAgentIterator->second;
}

std::string MetaRobotsHelpers::userAgentString(WellKnownUserAgent wellKnownUserAgent)
{
    const auto userAgentIterator = s_userAgentByType.find(wellKnownUserAgent);

    if (userAgentIterator == s_userAgentByType.end())
    {
        throw std::runtime_error("Passed unknown parameter");
    }

    return userAgentIterator->second;
}

std::vector<WellKnownUserAgent> MetaRobotsHelpers::wellKnownUserAgents()
{
    return std::vector<WellKnownUserAgent>
    {
        WellKnownUserAgent::GoogleBot, WellKnownUserAgent::YandexBot, WellKnownUserAgent::MailRuBot,
            WellKnownUserAgent::MsnBot, WellKnownUserAgent::YahooBot, WellKnownUserAgent::AnyBot
    };
}

}