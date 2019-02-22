#include "robots_txt_tokenizer.h"
#include "meta_robots_helpers.h"
#include "robots_txt_token.h"
#include "robots_txt_tokenizer_impl.h"

namespace cpprobotparser
{

RobotsTxtTokenizer::RobotsTxtTokenizer()
    : m_impl(new RobotsTxtTokenizerImpl)
{
}

RobotsTxtTokenizer::RobotsTxtTokenizer(const RobotsTxtTokenizer& other)
    : m_impl(new RobotsTxtTokenizerImpl(*other.m_impl))
{
}

RobotsTxtTokenizer::RobotsTxtTokenizer(RobotsTxtTokenizer&& other)
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

const std::string&
RobotsTxtTokenizer::sitemapUrl() const
{
    return m_impl->sitemapUrl();
}

const std::string&
RobotsTxtTokenizer::originalHostMirrorUrl() const
{
    return m_impl->originalHostMirrorUrl();
}

bool RobotsTxtTokenizer::hasUserAgentRecord(WellKnownUserAgent userAgentType) const
{
    return m_impl->hasUserAgentRecord(userAgentType);
}

}
