#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include <gtest/gtest.h>
#include <string>
#include <locale>
#include <codecvt>
#include "robots_txt_rules.h"
#include "well_known_user_agent.h"

using namespace cpprobotparser;

TEST(RulesTests, SimpleRobotsTxt)
{
    const std::string robotsTxt = R"(
        User-agent: *
        Allow: /

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
        Clean-param: ref /some_dir/get_book.pl)";

    RobotsTxtRules rules(robotsTxt);

    //============================================ Rules for Yandex ============================================

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

    // all disallowed URLs for Google must be allowed for Yandex
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/oembed", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/oembed/", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/oembed/something_else", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/forks", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/forks/", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/forks/2", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/issues/new", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/issues/new/", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/issues/new/3/4/5", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/3/4", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/3/4/", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/3/4/5", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/mypage1.php?author", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/mypage2.php?author", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/blob", WellKnownUserAgent::YandexBot), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/blob/3/4", WellKnownUserAgent::YandexBot), true);

    //============================================ Rules for Google ==============================================

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

    //===================================== Rules for any other robots ==============================================

    // All URLs disallowed for Google and Yandex must be allowed for any other robots
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog/", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog/1", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/catalog/1/2/3/4/5", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/oembed", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/oembed/", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/oembed/something_else", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/forks", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/forks/", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/forks/2", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/issues/new", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/issues/new/", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/issues/new/3/4/5", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/3/4", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/3/4/", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/3/4/5", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/mypage1.php?author", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/commits/mypage2.php?author", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/blob", WellKnownUserAgent::AllRobots), true);
    EXPECT_EQ(rules.isUrlAllowed("http://www.example.com/1/2/blob/3/4", WellKnownUserAgent::AllRobots), true);
}

TEST(RulesTests, RegularExpressionsRobotsTxt)
{
    const std::string robotsTxt = R"(
        User-agent: *
        Disallow: /api*html
        Disallow: */api/*/ext$
        Disallow: *js$
        Disallow: /example*$)";

    RobotsTxtRules rules(robotsTxt);

    EXPECT_EQ(rules.isUrlAllowed("http://a.com/js.js", WellKnownUserAgent::GoogleBot), false); // Disallow : *js$
    EXPECT_EQ(rules.isUrlAllowed("http://a.com/a/b/js.js", WellKnownUserAgent::GoogleBot), false); // Disallow : *js$
    EXPECT_EQ(rules.isUrlAllowed("http://a.com/js.jsa", WellKnownUserAgent::GoogleBot), true); // // Disallow : *js$

    EXPECT_EQ(rules.isUrlAllowed("http://a.com/a/api/a/b/js.extcss", WellKnownUserAgent::GoogleBot), true); // Disallow : */api/*/ext$
    EXPECT_EQ(rules.isUrlAllowed("http://a.com/a/api/a/b/ext", WellKnownUserAgent::GoogleBot), false); // Disallow : */api/*/ext$
    EXPECT_EQ(rules.isUrlAllowed("http://a.com/a/c/a/b/js.ext", WellKnownUserAgent::GoogleBot), true); // Disallow : */api/*/ext$
    EXPECT_EQ(rules.isUrlAllowed("http://a.com/ext", WellKnownUserAgent::GoogleBot), true); // Disallow : */api/*/ext$


    EXPECT_EQ(rules.isUrlAllowed("http://a.com/a/api/a/b/js.html", WellKnownUserAgent::GoogleBot), false); // Disallow : /api*html
    EXPECT_EQ(rules.isUrlAllowed("http://a.com/a/api/a/b/html/a/b/c/d.css", WellKnownUserAgent::GoogleBot), false); // Disallow : /api*html
    EXPECT_EQ(rules.isUrlAllowed("http://a.com/a/api/a/b/js.htmlext", WellKnownUserAgent::GoogleBot), false); // Disallow : /api*html
    EXPECT_EQ(rules.isUrlAllowed("http://a.com/a/a/b/js.htmlext", WellKnownUserAgent::GoogleBot), true); // Disallow : /api*html
    EXPECT_EQ(rules.isUrlAllowed("http://a.com/index.html", WellKnownUserAgent::GoogleBot), true); // Disallow : /api*html

    EXPECT_EQ(rules.isUrlAllowed("http://a.com/example/index.html", WellKnownUserAgent::GoogleBot), true); // Disallow: /example*$
    EXPECT_EQ(rules.isUrlAllowed("http://a.com/example", WellKnownUserAgent::GoogleBot), true); // Disallow: /example*$
}

TEST(RulesTests, DifferentLineSeparatorsRobotsTxt)
{
    const std::string robotsTxtRN =
        "User-agent: *\r\n"
        "Disallow : /api*html\r\n";

    RobotsTxtRules rulesRN(robotsTxtRN);

    EXPECT_EQ(rulesRN.isUrlAllowed("http://a.com/api/index.html", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rulesRN.isUrlAllowed("http://a.com/index.html", WellKnownUserAgent::GoogleBot), true);

    const std::string robotsTxtNR =
        "User-agent: *\n\r"
        "Disallow : /api*html\n\r";

    RobotsTxtRules rulesNR(robotsTxtNR);

    EXPECT_EQ(rulesNR.isUrlAllowed("http://a.com/api/index.html", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rulesNR.isUrlAllowed("http://a.com/index.html", WellKnownUserAgent::GoogleBot), true);

    const std::string robotsTxtR =
        "User-agent: *\r"
        "Disallow : /api*html\r";

    RobotsTxtRules rulesR(robotsTxtR);

    EXPECT_EQ(rulesR.isUrlAllowed("http://a.com/api/index.html", WellKnownUserAgent::GoogleBot), false);
    EXPECT_EQ(rulesR.isUrlAllowed("http://a.com/index.html", WellKnownUserAgent::GoogleBot), true);
}

TEST(RulesTests, InvalidRobotsTxt)
{
    // no user-agent
    const std::string robotsTxt = "Disallow : /api*html\n";
    RobotsTxtRules rules(robotsTxt);

    EXPECT_EQ(rules.isUrlAllowed("http://a.com/api/index.html", WellKnownUserAgent::GoogleBot), true);
}

TEST(RulesTests, BadPatternsRobotsTxt)
{
    // TODO

    /*
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    const std::string robotsTxt = converter.to_bytes(LR"(
        User-agent: *
        Disallow: /api*htm$l
        Disallow: /ъю№чшэр)");

    RobotsTxtRules rules(robotsTxt);

    // is used variable in order to suppress the MSVC warnings
    const std::string cyrillicUrl = converter.to_bytes(L"http://a.com/ъю№чшэр");

    EXPECT_EQ(rules.isUrlAllowed("http://a.com/api/index.html", WellKnownUserAgent::GoogleBot), true); // Disallow : /api*htm$l
    EXPECT_EQ(rules.isUrlAllowed(cyrillicUrl, WellKnownUserAgent::GoogleBot), true); // "Disallow : /ъю№чшэр"
    */
}

TEST(RulesTests, NonAsciiSymbolsRobotsTxt)
{
    // TODO

    /*
    const std::string robotsTxt = R"(
        User-agent: *
        Disallow : /%D0%BA%D0%BE%D1%80%D0%B7%D0%B8%D0%BD%D0%B0)";

    RobotsTxtRules rules(robotsTxt);

    EXPECT_EQ(rules.isUrlAllowed("http://a.com/%D0%BA%D0%BE%D1%80%D0%B7%D0%B8%D0%BD%D0%B0", WellKnownUserAgent::GoogleBot), false);
    //EXPECT_EQ(rules.isUrlAllowed(L"http://a.com/ъю№чшэр", WellKnownUserAgent::GoogleBot), false);
    //EXPECT_EQ(rules.isUrlAllowed(L"http://a.com/ъЮ№чшэр", WellKnownUserAgent::GoogleBot), false);
    //EXPECT_EQ(rules.isUrlAllowed(L"http://a.com/ъР№чшэр", WellKnownUserAgent::GoogleBot), true);
    */
}
