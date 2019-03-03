#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "robots_txt_token.h"
#include "robots_txt_tokenizer.h"
#include "well_known_user_agent.h"

using namespace cpprobotparser;

namespace
{

const std::string s_testData(R"(
        Sitemap: www.example.com/sitemap.xml

        User-agent: *
        Crawl-delay: 2.0

        User-agent: Googlebot
        Disallow: /oembed
        Disallow: /*/forks
        Allow: /*/*/tree/master
        Allow: /*/*/blob/master

        User-agent: Yandex
        Allow: /
        Allow: /catalog/auto
        Disallow: /catalog
        Clean-param: ref /some_dir/get_book.pl
        )");

}

TEST(TokenizerTests, ParseSiteMap)
{
    RobotsTxtTokenizer tokenizer(s_testData);
    const std::string sitemapUrl = tokenizer.sitemapUrl();

    EXPECT_EQ(sitemapUrl, std::string_view("www.example.com/sitemap.xml"));
}

TEST(TokenizerTests, ParseCleanParamTokens)
{
    RobotsTxtTokenizer tokenizer(s_testData);

    const std::vector<std::string> yandexBotCleanParams =
        tokenizer.tokenValues(WellKnownUserAgent::YandexBot, RobotsTxtToken::TokenCleanParam);

    const std::vector<std::string> googleBotCleanParams =
        tokenizer.tokenValues(WellKnownUserAgent::GoogleBot, RobotsTxtToken::TokenCleanParam);

    GTEST_ASSERT_EQ(yandexBotCleanParams.size(), 1);
    EXPECT_EQ(yandexBotCleanParams.front(), std::string_view("ref /some_dir/get_book.pl"));
    EXPECT_EQ(googleBotCleanParams.empty(), true);
}

TEST(TokenizerTests, ParseAllowTokens)
{
    RobotsTxtTokenizer tokenizer(s_testData);

    const std::vector<std::string> yandexBotAllowTokens =
        tokenizer.tokenValues(WellKnownUserAgent::YandexBot, RobotsTxtToken::TokenAllow);

    const std::vector<std::string> googleBotAllowTokens =
        tokenizer.tokenValues(WellKnownUserAgent::GoogleBot, RobotsTxtToken::TokenAllow);

    GTEST_ASSERT_EQ(yandexBotAllowTokens.size(), 2);
    GTEST_ASSERT_EQ(googleBotAllowTokens.size(), 2);

    const std::vector<std::string> yandexAllowTokens
    {
        "/",
        "/catalog/auto"
    };
    const std::vector<std::string> googleAllowTokens
    {
        "/*/*/tree/master",
        "/*/*/blob/master"
    };

    for (const std::string& allowToken : yandexBotAllowTokens)
    {
        EXPECT_NE(std::find(yandexAllowTokens.begin(), yandexAllowTokens.end(), allowToken), yandexAllowTokens.end());
    }
    for (const std::string& allowToken : googleBotAllowTokens)
    {
        EXPECT_NE(std::find(googleAllowTokens.begin(), googleAllowTokens.end(), allowToken), googleAllowTokens.end());
    }
}