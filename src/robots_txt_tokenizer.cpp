#include "robots_txt_tokenizer.h"
#include "robots_txt_token.h"
#include "robots_txt_tokenizer_impl.h"

cpprobotparser::RobotsTxtTokenizer operator "" _tokenizeRobotsTxt(const char* robotsTxtContent, std::size_t)
{
    return cpprobotparser::RobotsTxtTokenizer(robotsTxtContent);
}

namespace cpprobotparser
{

RobotsTxtTokenizer::RobotsTxtTokenizer()
    : m_impl(new RobotsTxtTokenizerImpl)
{
}

RobotsTxtTokenizer::RobotsTxtTokenizer(const std::string& robotsTxtContent)
    : RobotsTxtTokenizer()
{
    tokenize(robotsTxtContent);
}

RobotsTxtTokenizer::RobotsTxtTokenizer(const RobotsTxtTokenizer& other)
    : m_impl(new RobotsTxtTokenizerImpl(*other.m_impl))
{
}

RobotsTxtTokenizer::RobotsTxtTokenizer(RobotsTxtTokenizer&& other) noexcept
    : m_impl(other.m_impl)
{
    other.m_impl = nullptr;
}

RobotsTxtTokenizer& RobotsTxtTokenizer::operator=(const RobotsTxtTokenizer& other)
{
    if (this == std::addressof(other))
    {
        return *this;
    }

    m_impl = new RobotsTxtTokenizerImpl(*other.m_impl);
    return *this;
}

RobotsTxtTokenizer& RobotsTxtTokenizer::operator=(RobotsTxtTokenizer&& other)
{
    if (this == std::addressof(other))
    {
        return *this;
    }

    m_impl = other.m_impl;
    other.m_impl = nullptr;
    return *this;
}

RobotsTxtTokenizer::~RobotsTxtTokenizer()
{
    delete m_impl;
}

bool RobotsTxtTokenizer::isValid() const noexcept
{
    return m_impl->isValid();
}

void RobotsTxtTokenizer::tokenize(const std::string& robotsTxtContent)
{
    m_impl->tokenize(robotsTxtContent);
}

std::vector<std::string>
RobotsTxtTokenizer::tokenValues(WellKnownUserAgent userAgentType, RobotsTxtToken token) const
{
    return m_impl->tokenValues(userAgentType, token);
}

std::vector<std::string>
RobotsTxtTokenizer::tokenValues(const std::string& userAgent, RobotsTxtToken token) const
{
    return m_impl->tokenValues(userAgent, token);
}

const std::string&
RobotsTxtTokenizer::sitemapUrl() const noexcept
{
    return m_impl->sitemapUrl();
}

const std::string&
RobotsTxtTokenizer::originalHostMirrorUrl() const noexcept
{
    return m_impl->originalHostMirrorUrl();
}

bool RobotsTxtTokenizer::hasUserAgentRecord(WellKnownUserAgent userAgentType) const
{
    return m_impl->hasUserAgentRecord(userAgentType);
}

bool RobotsTxtTokenizer::hasUserAgentRecord(const std::string& userAgent) const
{
    return m_impl->hasUserAgentRecord(userAgent);
}

}
