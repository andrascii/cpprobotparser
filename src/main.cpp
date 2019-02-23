#include "robots_txt_tokenizer.h"
#include "meta_robots_helpers.h"
#include "well_known_user_agent.h"
#include "robots_txt_token.h"
#include "robots_txt_rules.h"

using namespace cpprobotparser;

int main(int, char**)
{
    RobotsTxtTokenizer tokenizer =
        R"(
        Sitemap: www.example.com/sitemap.xml

        User-agent: *
        Crawl-delay: 2.0

        User-agent: Googlebot

        Disallow: /oembed
        Disallow: /*/forks
        Disallow: /*/stars
        Disallow: /*/download
        Disallow: /*/revisions
        Disallow: /*/*/issues/new
        Disallow: /*/*/issues/search
        Disallow: /*/*/commits/*/*
        Disallow: /*/*/commits/*?author
        Disallow: /*/*/commits/*?path
        Disallow: /*/*/branches
        Disallow: /*/*/tags
        Disallow: /*/*/contributors
        Disallow: /*/*/comments

        Allow: /*/*/tree/master
        Allow: /*/*/blob/master

        User-agent: Yandex
        Allow: *
        Clean-param: ref /some_dir/get_book.pl
        )"_tokenizeRobotsTxt;

    const std::string sitemapUrl = tokenizer.sitemapUrl();

    std::cout << "Site map URL: " << (sitemapUrl.empty() ? "None" : sitemapUrl) << "\n\n";

    const std::vector<WellKnownUserAgent> wellKnownUserAgents =
        MetaRobotsHelpers::wellKnownUserAgents();

    for (WellKnownUserAgent wellKnownUserAgent : wellKnownUserAgents)
    {
        const std::vector<std::string> allowTokens =
            tokenizer.tokenValues(wellKnownUserAgent, RobotsTxtToken::TokenAllow);

        const std::vector<std::string> disallowTokens =
            tokenizer.tokenValues(wellKnownUserAgent, RobotsTxtToken::TokenDisallow);

        const std::vector<std::string> cleanParamTokens =
            tokenizer.tokenValues(wellKnownUserAgent, RobotsTxtToken::TokenCleanParam);

        const std::vector<std::string> crawlDelayTokens =
            tokenizer.tokenValues(wellKnownUserAgent, RobotsTxtToken::TokenCrawlDelay);

        if (allowTokens.empty() &&
            disallowTokens.empty() &&
            cleanParamTokens.empty() &&
            crawlDelayTokens.empty())
        {
            continue;
        }

        std::cout << "Tokens for " << MetaRobotsHelpers::userAgentString(wellKnownUserAgent) << ": \n";

        // print all allow tokens
        for (const std::string& allowTokenValue : allowTokens)
        {
            std::cout << "Allow: " << allowTokenValue << "\n";
        }

        // print all disallow tokens
        for (const std::string& disallowTokenValue : disallowTokens)
        {
            std::cout << "Disallow: " << disallowTokenValue << "\n";
        }

        // print all clean-param tokens
        for (const std::string& cleanParamTokenValue : cleanParamTokens)
        {
            std::cout << "Clean-param: " << cleanParamTokenValue << "\n";
        }

        // print all crawl-delay tokens
        for (const std::string& crawlDelayTokenValue : crawlDelayTokens)
        {
            std::cout << "Crawl-delay: " << crawlDelayTokenValue << "\n";
        }

        std::cout << "\n\n";
    }

    //////////////////////////////////////////////////////////////////////////

    RobotsTxtRules rules(R"(
        Sitemap: www.example.com/sitemap.xml

        User-agent: *
        Crawl-delay: 2.0

        User-agent: Googlebot

        Disallow: /oembed
        Disallow: /*/forks
        Disallow: /*/stars
        Disallow: /*/download
        Disallow: /*/revisions
        Disallow: /*/*/issues/new
        Disallow: /*/*/issues/search
        Disallow: /*/*/commits/*/*
        Disallow: /*/*/commits/*?author
        Disallow: /*/*/commits/*?path
        Disallow: /*/*/branches
        Disallow: /*/*/tags
        Disallow: /*/*/contributors
        Disallow: /*/*/comments

        Allow: /*/*/tree/master
        Allow: /*/*/blob/master

        User-agent: Yandex
        Allow: /
        Allow: /catalog/auto
        Disallow: /catalog
        Clean-param: ref /some_dir/get_book.pl
        )");

    std::cout << std::boolalpha << "expected false: " << rules.isUrlAllowed("http://www.example.com/catalog", WellKnownUserAgent::YandexBot) << "\n";
    std::cout << std::boolalpha << "expected true: " << rules.isUrlAllowed("http://www.example.com/catalog/auto", WellKnownUserAgent::YandexBot) << "\n";
    std::cout << std::boolalpha << "expected true: " << rules.isUrlAllowed("http://www.example.com/", WellKnownUserAgent::YandexBot) << "\n";
    std::cout << std::boolalpha << "expected false: " << rules.isUrlAllowed("http://www.example.com/index/folder/issues/new/", WellKnownUserAgent::GoogleBot) << "\n";

    std::cin.get();
}