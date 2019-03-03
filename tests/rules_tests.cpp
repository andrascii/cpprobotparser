#include <gtest/gtest.h>
#include <string>
#include "robots_txt_rules.h"
#include "well_known_user_agent.h"

using namespace cpprobotparser;

TEST(RulesTests, isAllowedTest)
{
    const std::string testData(R"(
        Sitemap: www.example.com/sitemap.xml

        User-agent: *
        Crawl-delay: 2.0

        User-agent: Googlebot

        Disallow: /oembed
        Disallow: /*/forks
        Disallow: /*/*/issues/new
        Disallow: /*/*/commits/*/*
        Disallow: /*/*/commits/*?author
        Disallow: /*/*/blob

        Allow: /*/*/tree/master
        Allow: /*/*/blob/master

        User-agent: Yandex
        Allow: /
        Allow: /catalog/auto
        Disallow: /catalog
        Clean-param: ref /some_dir/get_book.pl
        )");

    RobotsTxtRules rules(testData);

    // Rules for Yandex

    // must be disallowed
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog", WellKnownUserAgent::YandexBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog/", WellKnownUserAgent::YandexBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog/1", WellKnownUserAgent::YandexBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog/1/2/3/4/5", WellKnownUserAgent::YandexBot), false);

    // must be allowed
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog/auto", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog/auto/", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog/auto/1", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog/auto/1/2/3/4/5", WellKnownUserAgent::YandexBot), true);

    // Rules for Google

    // must be disallowed
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/oembed", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/oembed/", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/oembed/something_else", WellKnownUserAgent::GoogleBot), false);

    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/forks", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/forks/", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/forks/2", WellKnownUserAgent::GoogleBot), false);

    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/issues/new", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/issues/new/", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/issues/new/3/4/5", WellKnownUserAgent::GoogleBot), false);

    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/3/4", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/3/4/", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/3/4/5", WellKnownUserAgent::GoogleBot), false);

    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/mypage1.php?author", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/mypage2.php?author", WellKnownUserAgent::GoogleBot), false);

    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/blob", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/blob/3/4", WellKnownUserAgent::GoogleBot), false);

    // must be allowed
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/3", WellKnownUserAgent::GoogleBot), true);

    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/commits/mypage2.php?author", WellKnownUserAgent::GoogleBot), true);

    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/blob/master", WellKnownUserAgent::GoogleBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/blob/master/3/4", WellKnownUserAgent::GoogleBot), true);

    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/tree/master", WellKnownUserAgent::GoogleBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/tree/master/3/4", WellKnownUserAgent::GoogleBot), true);
}