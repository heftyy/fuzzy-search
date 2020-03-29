#include "FuzzySearch.h"

#include <numeric>

// #define USE_STD_LOWER
#if defined(USE_STD_LOWER)
#include <cctype>
#endif

namespace FuzzySearch
{

constexpr int start_score = 5;               // every match starts out with this
constexpr int sequential_bonus = 20;         // bonus for adjacent matches
constexpr int separator_bonus = 20;          // bonus if match occurs after a separator
constexpr int camel_bonus = 25;              // bonus if match is uppercase and prev is lower
constexpr int first_letter_bonus = 25;       // bonus if the first letter is matched
constexpr int filename_bonus = 15;           // bonus if the match is in the filename instead of the path
constexpr int leading_letter_penalty = -2;   // penalty applied for every letter in str before the first match
constexpr int unmatched_letter_penalty = -1; // penalty for every letter that doesn't matter

constexpr int max_leading_letter_penalty = -10;
constexpr int max_unmatched_characters_from_pattern = 2;

constexpr uint8_t character_to_lower = 0x20U;

constexpr bool IsLower(unsigned char c) noexcept
{
#if defined(USE_STD_LOWER)
	return std::islower(c);
#else
	return (c & character_to_lower) != 0;
#endif
}

constexpr bool IsUpper(unsigned char c) noexcept
{
#if defined(USE_STD_LOWER)
	return std::isupper(c);
#else
	return (c & character_to_lower) == 0;
#endif
}

constexpr unsigned int ToLower(unsigned int c) noexcept
{
#if defined(USE_STD_LOWER)
	return std::tolower(c);
#else
	return c | character_to_lower;
#endif
}

inline bool IsSourceFile(std::string_view str)
{
	if (str[str.length() - 4] == '.')
	{
		const size_t extension_start = str.length() - 4;
		if (std::strncmp(str.data() + extension_start, "cpp", 3) != 0)
		{
			return true;
		}
	}
	else if (str[str.length() - 3] == '.')
	{
		const size_t extension_start = str.length() - 3;
		if (std::strncmp(str.data() + extension_start, "py", 2) != 0)
		{
			return true;
		}

		if (std::strncmp(str.data() + extension_start, "cs", 2) != 0)
		{
			return true;
		}
	}
	else if(str[str.length() - 2] == '.')
	{
		if (str[str.length() - 1] == 'c')
		{
			return true;
		}
	}

	return false;
}

template<typename T>
class Span
{
public:
	using const_iterator = const T*;

	explicit Span(const T* data, size_t size)
	{
		m_Data = data;
		m_Size = size;
	}

	[[nodiscard]] const_iterator begin() const { return m_Data; }
	[[nodiscard]] const_iterator end() const { return m_Data + m_Size; }

	const T& operator[] (size_t index) const { return m_Data[index]; }

	[[nodiscard]] size_t size() const { return m_Size; }

private:
	const T* m_Data;
	size_t m_Size;
};

int CalculateSequentialMatchScore(std::string_view str, int filename_start_index, MatchMode match_mode, const Span<int>& matches)
{
	int out_score = start_score;
	const int str_length = static_cast<int>(str.length());

	int matches_in_filename = 0;
	int first_match_in_filename = -1;

	// Apply ordering bonuses
	for (const int curr_index : matches)
	{
		// Check for bonuses based on neighbor character value
		if (curr_index > 0 && (match_mode == MatchMode::E_FILENAMES || match_mode == MatchMode::E_SOURCE_FILES))
		{
			const char neighbor = str[curr_index - 1];
			const char curr = str[curr_index];

			// Camel case
			if ((IsLower(neighbor) || neighbor == '\\' || neighbor == '/') && IsUpper(curr))
			{
				out_score += camel_bonus;
			}

			// Separator
			if (neighbor == '_' || neighbor == ' ' || neighbor == '\\' || neighbor == '/')
			{
				out_score += separator_bonus;
			}
		}

		if (curr_index >= filename_start_index)
		{
			// Save the first match in the filename
			if (first_match_in_filename == -1)
			{
				first_match_in_filename = curr_index;
			}

			// Bonus for matching the filename
			out_score += filename_bonus;
			if (curr_index == filename_start_index)
			{
				// First letter
				out_score += first_letter_bonus;
			}
			++matches_in_filename;
		}
	}

	if (match_mode == MatchMode::E_SOURCE_FILES && IsSourceFile(str))
	{
		out_score += 2;
	}

	// Apply leading letter penalty
	const int calculated_leading_letter_penalty = std::min(leading_letter_penalty * (first_match_in_filename - filename_start_index), 0);
	out_score += std::max(calculated_leading_letter_penalty, max_leading_letter_penalty);

	// Apply unmatched penalty
	const int unmatched = (str_length - filename_start_index - matches_in_filename) / 3;
	out_score += std::min(unmatched_letter_penalty * unmatched, 0);

	// Apply sequential match bonus
	out_score += sequential_bonus * (static_cast<int>(matches.size()) - 1);

	return out_score;
}

inline int FindSequentialMatch(std::string_view pattern, int pattern_index, std::string_view str, int str_index) noexcept
{
	// The pattern characters usually mismatch str characters so this early out just helps optizmier/cpu
	if (ToLower(pattern[pattern_index]) != ToLower(str[str_index]))
	{
		return 0;
	}

	const int pattern_length = static_cast<int>(pattern.length());
	const int str_length = static_cast<int>(str.length());

	int matched_chars = 0;
	while (ToLower(pattern[pattern_index + matched_chars]) == ToLower(str[str_index + matched_chars]))
	{
		++matched_chars;
		if (pattern_index + matched_chars >= pattern_length || str_index + matched_chars >= str_length)
		{
			return matched_chars;
		}
	}

	return matched_chars;
}

PatternMatch CalculatePatternScore(std::string_view pattern, const Span<PatternMatch>& in_matches)
{
	PatternMatch out_match;
	out_match.m_Matches.reserve(pattern.length());

	const int pattern_length = static_cast<int>(pattern.length());

	for (int pattern_index = 0; pattern_index < pattern_length; ++pattern_index)
	{
		const PatternMatch& match = in_matches[pattern_index];
		if (match.m_Score > 0)
		{
			out_match.m_Score += match.m_Score;
			out_match.m_Matches.insert(out_match.m_Matches.end(), match.m_Matches.begin(), match.m_Matches.end());

			// Advance the pattern_index by the match length, m_Score is only set for the first character of a match
			pattern_index += static_cast<int>(match.m_Matches.size() - 1);
		}
	}

	return out_match;
}

PatternMatch FuzzyMatch(InputPattern& input_pattern, std::string_view str, MatchMode match_mode)
{
	std::string_view pattern = input_pattern.m_Pattern;

	const int pattern_length = static_cast<int>(pattern.length());
	const int str_length = static_cast<int>(str.length());

	std::vector<PatternMatch>& pattern_matches = input_pattern.m_PatternMatches;
	std::vector<int>& match_indexes = input_pattern.m_MatchIndexes;

	int last_path_separator_index = 0;
	if (match_mode == MatchMode::E_SOURCE_FILES || match_mode == MatchMode::E_FILENAMES)
	{
		last_path_separator_index = static_cast<int>(str.find_last_of("\\/"));
	}

	int str_start = 0;
	int unmatched_characters_from_pattern = 0;

	// Loop through pattern and str looking for a match
	for (int pattern_index = 0; pattern_index < pattern_length; ++pattern_index)
	{
		pattern_matches[pattern_index].m_Score = 0;

		int best_match_length = 0;

		// When pattern contains a space, start a search from the beginning of str
		// again to allow out of order matches from the pattern
		if (pattern[pattern_index] == ' ')
		{
			str_start = 0;
			continue;
		}

		for (int str_index = str_start; str_index < str_length; ++str_index)
		{
			const int match_length = FindSequentialMatch(pattern, pattern_index, str, str_index);
			if (match_length > 0)
			{
				// We know that the sequential match started at str_index so fill match_indexes
				std::iota(match_indexes.begin(), match_indexes.begin() + match_length, str_index);

				const auto match_span = Span<int>(match_indexes.data(), match_length);
				const int match_score = CalculateSequentialMatchScore(str, last_path_separator_index, match_mode, match_span);

				if (match_score > pattern_matches[pattern_index].m_Score)
				{
					best_match_length = match_length;

					PatternMatch& match = pattern_matches[pattern_index];
					match.m_Score = match_score;
					match.m_Matches.assign(match_indexes.begin(), match_indexes.begin() + best_match_length);

					// Skip searching for matches in str that we already used in our currect best match, doing this to improve performance
					// -1 because we will increment str_index at the end of the loop
					str_index += best_match_length - 1;
					str_start = str_index;
				}
			}
		}

		if (pattern_matches[pattern_index].m_Score > 0)
		{
			pattern_index += best_match_length - 1;
		}
		else if (best_match_length == 0)
		{
			++unmatched_characters_from_pattern;
			// Allow some unmatched characters (typos etc...)
			if (unmatched_characters_from_pattern >= max_unmatched_characters_from_pattern)
			{
				return {0};
			}
		}
	}

	return CalculatePatternScore(pattern, Span<PatternMatch>(pattern_matches.data(), pattern_length));
}

bool SearchResultComparator(const SearchResult& lhs, const SearchResult& rhs) noexcept
{
	if (lhs.m_PatternMatch.m_Score > rhs.m_PatternMatch.m_Score)
	{
		return true;
	}
	if (lhs.m_PatternMatch.m_Score == rhs.m_PatternMatch.m_Score)
	{
		return lhs.m_String.size() < rhs.m_String.size();
	}
	return false;
}

} // namespace FuzzySearch
