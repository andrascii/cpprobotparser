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

    RobotsTxtRules rules(testData);

    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog", WellKnownUserAgent::YandexBot), false);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog/auto", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/index/folder/issues/new/", WellKnownUserAgent::GoogleBot), false);
}