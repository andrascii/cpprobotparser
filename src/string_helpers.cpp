#include "string_helpers.h"

namespace
{

auto toLowerLambdaHelper()
{
    return [](char ch)
    {
        return static_cast<char>(std::tolower(ch));
    };
}

}

namespace cpprobotparser
{

void StringHelpers::toLower(std::string& source)
{
    std::transform(source.begin(), source.end(), source.begin(), toLowerLambdaHelper());
}

std::string StringHelpers::toLower(const std::string& source)
{
    std::string result;
    std::transform(source.begin(), source.end(), std::inserter(result, result.end()), toLowerLambdaHelper());

    return std::move(result);
}

std::string StringHelpers::removeAllFrom(const std::string& source, const std::string& substring)
{
    const auto pos = source.find(substring);

    if (pos != std::string::npos)
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

    const auto range = spaceStringBounds(source);
    const size_t lowerBound = range.first;
    const size_t upperBound = range.second;

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

    const auto range = spaceStringBounds(source);
    const size_t lowerBound = range.first;
    const size_t upperBound = range.second;

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
        const std::string copy = toLower(source);
        const std::string copySep = toLower(sep);

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
    //
    // TODO: don't work
    //
    StringList list;
    std::smatch match;

    if (std::regex_search(source, match, regularExpression))
    {
        for (const std::ssub_match& subMatch : match)
        {
            list.push_back(subMatch);
        }
    }

    if (behavior == SkipEmptyParts)
    {
        list.erase(std::remove_if(list.begin(), list.end(), [](const std::string& str) { return str.empty(); }), list.end());
    }

    return list;
}

StringHelpers::StringList
StringHelpers::splitString(
    const std::string& source,
    const std::function<bool(char)>& separatePredicate,
    SplitBehavior behavior)
{
    return findHelper(source, separatePredicate, behavior);
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
        list.push_back(source.substr(start, static_cast<Offset>(-1)));
    }

    return list;
}

StringHelpers::StringList
StringHelpers::findHelper(
    const std::string& source,
    const std::function<bool(char)>& separatePredicate,
    SplitBehavior behavior)
{
    using Offset = std::string::size_type;

    StringList list;

    auto start = source.begin();
    auto end = source.end();

    while ((end = std::find_if(start, source.end(), separatePredicate)) != source.end())
    {
        if (start != end || behavior == SplitBehavior::KeepEmptyParts)
        {
            list.push_back(std::string(start, end));
        }

        start = end + 1;
    }

    if (start != source.end() || behavior == SplitBehavior::KeepEmptyParts)
    {
        list.push_back(std::string(start, source.end()));
    }

    return list;
}

std::pair<size_t, size_t>
StringHelpers::spaceStringBounds(const std::string& source)
{
    const size_t length = source.length();
    size_t begin = 0;
    size_t end = source.size() - 1;

    int beginPointsToSpace = std::isspace(source[begin]);
    int endPointsToSpace = std::isspace(source[end]);

    while (begin < length && end > 0 && (beginPointsToSpace != 0 || endPointsToSpace != 0))
    {
        if (beginPointsToSpace != 0)
        {
            ++begin;
            beginPointsToSpace = std::isspace(source[begin]);
        }

        if (endPointsToSpace != 0)
        {
            --end;
            endPointsToSpace = std::isspace(source[end]);
        }
    }

    return std::make_pair(begin, end);
}

}