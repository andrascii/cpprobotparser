# cpprobotparser
This module represents the robots.txt parser written in C++, which answers questions about whether or not a particular user agent can fetch a URL on the Web site that published the robots.txt file. For more details on the structure of robots.txt files, see http://www.robotstxt.org/orig.html.

## RobotsTxtTokenizer

[`robots_txt_tokenizer.h`](https://github.com/andrascii/cpprobotparser/blob/master/include/robots_txt_tokenizer.h) - this tokenizer is used to only parse robots.txt file and provide you rules for each user agent occurred in the file.

Using `tokenValues` method it can give the value of any token taken from this enumeration [`robots_txt_token.h`](https://github.com/andrascii/cpprobotparser/blob/master/include/robots_txt_token.h):

```cpp
namespace cpprobotparser
{

enum class RobotsTxtToken
{
    TokenUserAgent,
    TokenAllow,
    TokenDisallow,
    TokenSitemap,
    TokenHost,
    TokenCrawlDelay,
    TokenCleanParam,
    TokenCommentary,
    TokenStringDelimeter,
    TokenUnknown
};

}
```

### Example

```cpp
#include "robots_txt_tokenizer.h"
#include "meta_robots_helpers.h"
#include "well_known_user_agent.h"
#include "robots_txt_token.h"

using namespace cpprobotparser;

int main(int, char**)
{
    RobotsTxtTokenizer tokenizer =
        "Sitemap: www.example.com/sitemap.xml\n \
        User-agent: Googlebot\n \
        Disallow: /oembed\n \
        Disallow: /*/forks\n \
        Disallow: /*/stars\n \
        Disallow: /*/download\n \
        Disallow: /*/revisions\n \
        Disallow: /*/*/issues/new\n \
        Disallow: /*/*/issues/search\n \
        Disallow: /*/*/commits/*/*\n \
        Disallow: /*/*/commits/*?author\n \
        Disallow: /*/*/commits/*?path\n \
        Disallow: /*/*/branches\n \
        Disallow: /*/*/tags\n \
        Disallow: /*/*/contributors\n \
        Disallow: /*/*/comments\n \
        Allow: /*/*/tree/master\n \
        Allow: /*/*/blob/master\n"_tokenizeRobotsTxt;

    const std::string sitemapUrl = tokenizer.sitemapUrl();
    const std::string originalHostMirrorUrl = tokenizer.originalHostMirrorUrl();

    std::cout << "Site map URL: " << (sitemapUrl.empty() ? "None" : sitemapUrl) << "\n";
    std::cout << "Original host mirror URL: " << (originalHostMirrorUrl.empty() ? "None" : originalHostMirrorUrl) << "\n";

    const std::vector<WellKnownUserAgent> wellKnownUserAgents =
        MetaRobotsHelpers::wellKnownUserAgents();

    for (WellKnownUserAgent wellKnownUserAgent : wellKnownUserAgents)
    {
        const std::vector<std::string> allowTokens =
            tokenizer.tokenValues(wellKnownUserAgent, RobotsTxtToken::TokenAllow);

        const std::vector<std::string> disallowTokens =
            tokenizer.tokenValues(wellKnownUserAgent, RobotsTxtToken::TokenDisallow);

        if (allowTokens.empty() && disallowTokens.empty())
        {
            continue;
        }

        std::cout << "Tokens for " << MetaRobotsHelpers::userAgentString(wellKnownUserAgent) << ": \n";

        for (const std::string& allowTokenValue : allowTokens)
        {
            std::cout << "Allow: " << allowTokenValue << "\n";
        }
        for (const std::string& disallowTokenValue : disallowTokens)
        {
            std::cout << "Disallow: " << disallowTokenValue << "\n";
        }
        std::cout << "*************************************************************************\n\n";
    }

    std::cin.get();
}
```
