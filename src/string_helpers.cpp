#include "string_helpers.h"

namespace cpprobotparser
{

std::string StringHelpers::removeAllFrom(const std::string& source, const std::string_view& substring)
{
    if (auto pos = source.find(substring); pos != std::string::npos)
    {
        return source.substr(0, pos);
    }

    return source;
}

void StringHelpers::trim(std::string& source)
{
    if (source.empty())
    {
        return;
    }

    auto[lowerBound, upperBound] = spaceStringBounds(source);

    if (lowerBound == source.length() && upperBound == 0)
    {
        source.clear();
    }
    else
    {
        source = std::move(source.substr(lowerBound, upperBound - lowerBound + 1));
    }
}

std::string StringHelpers::trimmed(const std::string& source)
{
    if (source.empty())
    {
        return source;
    }

    auto[lowerBound, upperBound] = spaceStringBounds(source);

    if (lowerBound == source.length() && upperBound == 0)
    {
        return std::string();
    }

    return source.substr(lowerBound, upperBound - lowerBound + 1);
}

StringHelpers::StringList
StringHelpers::splitString(
    const std::string& source,
    const std::string& sep,
    SplitBehavior behavior,
    CaseSensitivity cs)
{
    if (cs == CaseInsensitive)
    {
        std::string copy = source;
        std::tolower(copy.data(), copy.data() + copy.length());
        std::string copySep = sep;
        std::tolower(copySep.data(), copySep.data() + copySep.length());

        return findHelper(copy, copySep, behavior);
    }

    return findHelper(source, sep, behavior);
}

StringHelpers::StringList
StringHelpers::splitString(
    const std::string& source,
    const std::regex& regularExpression,
    SplitBehavior behavior)
{
    StringList list;
    std::smatch match;

    if (std::regex_match(source, match, regularExpression))
    {
        for (const sub_match& subMatch : match)
        {
            list.push_back(subMatch);
        }
    }

    return list;
}

StringHelpers::StringList
StringHelpers::findHelper(
    const std::string& source,
    const std::string& sep,
    SplitBehavior behavior)
{
    using Offset = std::string::size_type;

    StringList list;

    Offset start = 0;
    Offset end = std::string::npos;
    Offset offset = 0;

    while ((end = source.find(sep, start + offset)) != std::string::npos)
    {
        if (start != end || behavior == SplitBehavior::KeepEmptyParts)
        {
            list.push_back(source.substr(start, end - start));
        }

        start = end + sep.length();
        offset = (sep.length() == 0 ? 1 : 0);
    }

    if (start != source.size() || behavior == SplitBehavior::KeepEmptyParts)
    {
        list.push_back(source.substr(start, -1));
    }

    return list;
}

std::pair<size_t, size_t>
StringHelpers::spaceStringBounds(const std::string& source)
{
    const size_t length = source.length();
    size_t begin = 0;
    size_t end = source.size() - 1;

    bool beginPointsToSpace = std::isspace(source[begin]);
    bool endPointsToSpace = std::isspace(source[end]);

    while (begin < length && end > 0 && (beginPointsToSpace || endPointsToSpace))
    {
        if (beginPointsToSpace)
        {
            ++begin;
            beginPointsToSpace = std::isspace(source[begin]);
        }

        if (endPointsToSpace)
        {
            --end;
            endPointsToSpace = std::isspace(source[end]);
        }
    }

    return std::make_pair(begin, end);
}

}