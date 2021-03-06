﻿#include "string_helpers.h"

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
StringHelpers::split(
    const std::string& source,
    const std::string& sep,
    SplitBehavior behavior,
    CaseSensitivity cs)
{
    if (cs == CaseInsensitive)
    {
        const std::string copy = toLower(source);
        const std::string copySep = toLower(sep);

        return splitHelper(copy, copySep, behavior);
    }

    return splitHelper(source, sep, behavior);
}

StringHelpers::StringList
StringHelpers::split(
    const std::string& source,
    const std::regex& regularExpression,
    SplitBehavior behavior)
{
    StringList list;

    std::sregex_token_iterator tokenIterator(source.begin(), source.end(), regularExpression, -1);
    std::sregex_token_iterator end;

    for (; tokenIterator != end; ++tokenIterator)
    {
        list.push_back(*tokenIterator);
    }

    if (behavior == SkipEmptyParts)
    {
        list.erase(std::remove_if(list.begin(), list.end(), [](const std::string& str) { return str.empty(); }), list.end());
    }

    return list;
}

StringHelpers::StringList
StringHelpers::splitHelper(
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

bool StringHelpers::startsWith(const std::string& source, const std::string& substring, CaseSensitivity cs)
{
    if (cs == CaseInsensitive)
    {
        return startsWithHelper(toLower(source), toLower(substring));
    }

    return startsWithHelper(source, substring);
}

bool StringHelpers::endsWith(const std::string& source, const std::string& substring, CaseSensitivity cs)
{
    if (cs == CaseInsensitive)
    {
        return endsWithHelper(toLower(source), toLower(substring));
    }

    return endsWithHelper(source, substring);
}

bool StringHelpers::startsWithHelper(const std::string& source, const std::string& substring)
{
    return source.find(substring) == 0;
}

bool StringHelpers::endsWithHelper(const std::string& source, const std::string& substring)
{
    return source.rfind(substring) == source.size() - substring.size();
}

}