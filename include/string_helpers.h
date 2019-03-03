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

    static void toLower(std::string& source);
    static std::string toLower(const std::string& source);

    static std::string removeAllFrom(const std::string& source, const std::string& substring);

    static void trim(std::string& source);
    static std::string trimmed(const std::string& source);

    static StringList split(
        const std::string& source,
        const std::regex& regularExpression,
        SplitBehavior behavior);

    static StringList split(
        const std::string& source,
        const std::function<bool(char)>& separatePredicate,
        SplitBehavior behavior);

    static StringList split(
        const std::string& source,
        const std::string& sep,
        SplitBehavior behavior,
        CaseSensitivity cs = CaseSensitive);

    static bool startsWith(const std::string& source, const std::string& substring, CaseSensitivity cs = CaseSensitive);
    static bool endsWith(const std::string& source, const std::string& substring, CaseSensitivity cs = CaseSensitive);

private:
    static StringList findHelper(
        const std::string& source,
        const std::string& sep,
        SplitBehavior behavior);

    static StringList findHelper(
        const std::string& source,
        const std::function<bool(char)>& separatePredicate,
        SplitBehavior behavior);

    static std::pair<size_t, size_t> spaceStringBounds(
        const std::string& source);

    static bool startsWithHelper(const std::string& source, const std::string& substring);
    static bool endsWithHelper(const std::string& source, const std::string& substring);
};

}