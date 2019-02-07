#include "robots_txt_tokenizer.h"
#include "string_helpers.h"
#include "meta_robots_helpers.h"

using namespace cpprobotparser;

std::string content(const std::string& path)
{
    std::string result;
    std::ifstream in(path);

    while (in)
    {
        std::string line;
        std::getline(in, line);
        result += line + "\n";
    }

    return result;
}

int main(int argc, char** argv)
{
    std::string robotstxtContent;

    if (argc > 1)
    {
        robotstxtContent = content(argv[1]);

        RobotsTxtTokenizer tokenizer;
        tokenizer.tokenize(robotstxtContent);

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
    }
    else
    {
        std::cout << "Enter the path to the robots.txt file\n";
    }

    std::cin.get();
}