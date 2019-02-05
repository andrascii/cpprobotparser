#pragma once

namespace cpprobotparser
{

class StringHelpers
{
public:
    enum SplitBehavior
    {
        KeepEmptyParts,
        SkipEmptyParts
    };

    enum CaseSensitivity
    {
        CaseInsensitive,
        CaseSensitive
    };

    using StringList = std::vector<std::string>;

    static std::string removeAllFrom(const std::string& source, const std::string_view& substring);

    static void trim(std::string& source);
    static std::string trimmed(const std::string& source);

    static StringList splitString(
        const std::string& source,
        const std::regex& regularExpression,
        SplitBehavior behavior);

    static StringList splitString(
        const std::string& source,
        const std::string& sep,
        SplitBehavior behavior,
        CaseSensitivity cs);

private:
    static StringList findHelper(
        const std::string& source,
        const std::string& sep,
        SplitBehavior behavior);

    static std::pair<size_t, size_t> spaceStringBounds(
        const std::string& source);
};

}