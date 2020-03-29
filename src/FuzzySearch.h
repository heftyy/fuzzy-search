// I initially based this on
// https://blog.forrestthewoods.com/reverse-engineering-sublime-text-s-fuzzy-match-4cffeed33fdb
// https://medium.com/@Srekel/implementing-a-fuzzy-search-algorithm-for-the-debuginator-cacc349e6c55
// and ended up rewriting pretty much all of it from scrach to improve performance
// and changed match scores to fit my needs

#pragma once

#include <algorithm>
#include <string_view>
#include <type_traits>
#include <vector>

namespace FuzzySearch
{

struct PatternMatch
{
	int m_Score = 0;
	std::vector<int> m_Matches;
};

/*
 * InputPattern struct contains small helper buffers to avoid reallocating inside FuzzyMatch.
 * 
 * If you are going to search for the same pattern in multiple different strings
 * reuse the same instance of InputPattern for every search instead of recreating it.
*/
struct InputPattern
{
	explicit InputPattern(std::string_view pattern)
	    : m_Pattern(pattern)
	{
		m_PatternMatches.resize(pattern.length());
		m_MatchIndexes.resize(pattern.length());
	}

	std::string_view m_Pattern;

	std::vector<PatternMatch> m_PatternMatches;
	std::vector<int> m_MatchIndexes;
};

struct SearchResult
{
	std::string_view m_String;
	PatternMatch m_PatternMatch;
};

enum class MatchMode
{
	E_STRINGS,
	E_FILENAMES,
	E_SOURCE_FILES
};

constexpr size_t max_pattern_length = 1024;

PatternMatch FuzzyMatch(InputPattern& input_pattern, std::string_view str, MatchMode match_mode);

bool SearchResultComparator(const SearchResult& lhs, const SearchResult& rhs) noexcept;

template <typename Iterator, typename Func>
std::vector<SearchResult> Search(std::string_view pattern, Iterator&& begin, Iterator&& end, Func&& get_string_func, MatchMode match_mode)
{
	static_assert(
	    std::is_invocable_r<std::string_view, decltype(get_string_func), typename std::iterator_traits<Iterator>::value_type>::value,
	    "get_string_func needs to be a pointer to function returning std::string_view");

	if (pattern.empty())
	{
		return {};
	}
	else if (pattern.length() >= max_pattern_length)
	{
		pattern = pattern.substr(0, max_pattern_length);
	}

	std::vector<SearchResult> search_results;
	search_results.reserve(std::distance(begin, end) / 2);

	InputPattern input_pattern(pattern);

	std::for_each(begin, end, [&input_pattern, &get_string_func, match_mode, &search_results](const auto& element) {
		SearchResult search_result;
		search_result.m_String = get_string_func(element);
		search_result.m_PatternMatch = FuzzyMatch(input_pattern, search_result.m_String, match_mode);

		if (search_result.m_PatternMatch.m_Score > 0)
		{
			search_results.push_back(search_result);
		}
	});

	std::sort(search_results.begin(), search_results.end(), SearchResultComparator);

	return search_results;
}

} // namespace FuzzySearch