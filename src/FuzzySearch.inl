#include "FuzzySearch.h"

#include <string>
#include <numeric>
#include <cstring>
#include <algorithm>

namespace FuzzySearch
{
	constexpr int sequential_bonus = 20;         // bonus for adjacent matches
	constexpr int separator_bonus = 20;          // bonus if match occurs after a separator
	constexpr int camel_bonus = 30;              // bonus if match is uppercase and prev is lower
	constexpr int first_letter_bonus = 25;       // bonus if the first letter is matched
	constexpr int filename_bonus = 15;           // bonus if the match is in the filename instead of the path
	constexpr int whole_world_match_bonus = 20;  // bonus applied per character for matching the whole pattern in the searched string
	constexpr int leading_letter_penalty = -2;   // penalty applied for every letter in str before the first match
	constexpr int unmatched_letter_penalty = -1; // penalty for every letter that doesn't matter

	constexpr int max_leading_letter_penalty = -10;

	// std::string specialization
	template<> inline int FuzzySearchStringRef<std::string>::Length() const { return (int)m_String->length(); }
	template<> inline bool FuzzySearchStringRef<std::string>::Empty() const { return m_String->empty(); }

	template<> inline bool FuzzySearchStringRef<std::string>::Equals(int start, int length, const char* str) const { return m_String->compare(start, length, str) == 0; }
	template<> inline int FuzzySearchStringRef<std::string>::FindLastOf(const char* str) const
	{
		size_t found = m_String->find_last_of(str);
		if (found == std::string::npos)
		{
			return -1;
		}
		return (int)found;
	}

	template<> inline bool FuzzySearchStringRef<std::string>::IsLower(size_t index) const { return (m_String->at(index) & 0x20) != 0; }
	template<> inline int FuzzySearchStringRef<std::string>::ToLower(size_t index) const { return (m_String->at(index) | 0x20); }

	template<> inline int FuzzySearchStringRef<std::string>::operator[](size_t index) const { return m_String->at(index); }

	// cosnt char* specialization
	template<> inline int FuzzySearchStringRef<const char*>::Length() const { return (int)strlen(*m_String); }
	template<> inline bool FuzzySearchStringRef<const char*>::Empty() const { return Length() == 0; }

	template<> inline bool FuzzySearchStringRef<const char*>::Equals(int start, int length, const char* str) const { return std::strncmp(*m_String + start, str, length) == 0; }
	template<> inline int FuzzySearchStringRef<const char*>::FindLastOf(const char* str) const
	{
		for (int i = (int)strlen(*m_String) - 1; i >= 0; --i)
		{
			if (std::strchr(str, (*m_String)[i]) != nullptr)
				return i;
		}

		return -1;
	}

	template<> inline bool FuzzySearchStringRef<const char*>::IsLower(size_t index) const { return ((*m_String)[index] & 0x20) != 0; }
	template<> inline int FuzzySearchStringRef<const char*>::ToLower(size_t index) const { return ((*m_String)[index] | 0x20); }

	template<> inline int FuzzySearchStringRef<const char*>::operator[](size_t index) const { return (*m_String)[index]; }

	// fuzzy search impl

	template<typename String>
	inline bool IsSourceFile(const FuzzySearchStringRef<String>& str)
	{
		if (str[str.Length() - 4] == '.')
		{
			const int extension_start = str.Length() - 4;
			if (str.Equals(extension_start + 1, 3, "cpp"))
			{
				return true;
			}
		}
		else if (str[str.Length() - 3] == '.')
		{
			const int extension_start = str.Length() - 3;
			if (str.Equals(extension_start + 1, 2, "py"))
			{
				return true;
			}

			if (str.Equals(extension_start + 1, 2, "cs"))
			{
				return true;
			}
		}
		else if (str[str.Length() - 2] == '.')
		{
			if (str[str.Length() - 1] == 'c')
			{
				return true;
			}
		}

		return false;
	}

	template<typename String>
	inline bool IsSeparator(const FuzzySearchStringRef<String>& str, int index)
	{
		bool is_slash = index < 0 || index >= str.Length() || str[index] == '\\' || str[index] == '/';
		bool is_separator = is_slash || str[index] == '_' || str[index] == ' ';
		return is_separator;
	}

	template<typename String>
	inline int CalculateWholeWordMatch(const FuzzySearchStringRef<String>& pattern, int match_start, int match_length)
	{
		const int index_before_match = match_start - 1;
		const int index_after_match = match_start + match_length;
		const bool match_is_start_of_word = IsSeparator(pattern, index_before_match);
		const bool match_is_end_of_word = IsSeparator(pattern, index_after_match);

		if (match_is_start_of_word && match_is_end_of_word)
		{
			return whole_world_match_bonus * match_length;
		}

		return 0;
	}

	template<typename String>
	int CalculateSequentialMatchScore(const FuzzySearchStringRef<String>& str, int filename_start_index,  MatchMode match_mode, const std::vector<int>& matches, int match_length)
	{
		int out_score = 5;
		const int str_length = static_cast<int>(str.Length());

		int matches_in_filename = 0;
		int first_match_in_filename = -1;

		// Apply ordering bonuses
		for (int i = 0; i < match_length; ++i)
		{
			int curr_index = matches[i];
			// Check for bonuses based on neighbour character value
			if (match_mode == MatchMode::E_FILENAMES || match_mode == MatchMode::E_SOURCE_FILES)
			{
				int prev_index = curr_index - 1;
				bool is_prev_separator = IsSeparator(str, prev_index);
				bool is_prev_lower = is_prev_separator || str.IsLower(prev_index);
				bool is_curr_upper = !str.IsLower(curr_index);

				// Camel case
				if (is_prev_lower && is_curr_upper)
				{
					out_score += camel_bonus;
				}
				// Separator
				else if (is_prev_separator)
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
		out_score += sequential_bonus * (match_length - 1);

		return out_score;
	}

	template<typename String>
	inline int FindSequentialMatch(const FuzzySearchStringRef<String>& pattern, int pattern_index, const FuzzySearchStringRef<String>& str, int str_index) noexcept
	{
		// The pattern characters usually mismatch str characters so this early out just helps optizmier/cpu
		if (pattern.ToLower(pattern_index) != str.ToLower(str_index))
		{
			return 0;
		}

		const int pattern_length = static_cast<int>(pattern.Length());
		const int str_length = static_cast<int>(str.Length());

		int matched_chars = 0;
		while (pattern.ToLower(pattern_index + matched_chars) == str.ToLower(str_index + matched_chars))
		{
			++matched_chars;
			if (pattern_index + matched_chars >= pattern_length || str_index + matched_chars >= str_length)
			{
				return matched_chars;
			}
		}

		return matched_chars;
	}

	template<typename String>
	PatternMatch CalculatePatternScore(const FuzzySearchStringRef<String>& pattern, const std::vector<PatternMatch>& in_matches)
	{
		const int pattern_length = static_cast<int>(pattern.Length());

		PatternMatch out_match;
		out_match.m_Matches.reserve(pattern_length);

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

	template<typename String>
	PatternMatch FuzzyMatch(InputPattern<String>& input_pattern, const FuzzySearchStringRef<String>& str, SearchConfig search_config)
	{
		const FuzzySearchStringRef<String>& pattern = input_pattern.m_Pattern;

		const int pattern_length = static_cast<int>(pattern.Length());
		const int str_length = static_cast<int>(str.Length());

		std::vector<PatternMatch>& pattern_matches = input_pattern.m_PatternMatches;
		std::vector<int>& match_indexes = input_pattern.m_MatchIndexes;

		int filename_start_index = 0;
		if (search_config.m_MatchMode == MatchMode::E_SOURCE_FILES || search_config.m_MatchMode == MatchMode::E_FILENAMES)
		{
			auto found_separator = str.FindLastOf("\\/");
			if (found_separator != -1)
			{
				filename_start_index = static_cast<int>(found_separator) + 1;
			}
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

					int match_score = CalculateSequentialMatchScore(str, filename_start_index, search_config.m_MatchMode, match_indexes, match_length);

					// Apply whole word bonus if the match is a whole word from the pattern
					match_score += CalculateWholeWordMatch(pattern, pattern_index, match_length);

					if (match_score > pattern_matches[pattern_index].m_Score)
					{
						best_match_length = match_length;

						PatternMatch& match = pattern_matches[pattern_index];
						match.m_Score = match_score;
						match.m_Matches.assign(match_indexes.begin(), match_indexes.begin() + best_match_length);

						// Skip searching for matches in str that we already used in our currect best match, doing this to improve performance
						// -1 because we will increment str_index at the end of the loop
						str_index += best_match_length - 1;
						str_start = str_index + 1;
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
				if (unmatched_characters_from_pattern > search_config.m_MaxUnmatchedCharactersFromPattern)
				{
					return { 0, std::vector<int>() };
				}
			}
		}

		return CalculatePatternScore(pattern, pattern_matches);
	}

	template<typename String, typename Iterator, typename Func>
	std::vector<SearchResult<String>> Search(const String& pattern_str, Iterator begin, Iterator end, Func&& get_string_func, SearchConfig search_config)
	{
		InputPattern<String> input_pattern(pattern_str);
		if (input_pattern.m_Pattern.Empty())
		{
			return {};
		}

		std::vector<SearchResult<String>> search_results;
		search_results.reserve(std::distance(begin, end));

		std::for_each(begin, end, [&input_pattern, &get_string_func, search_config, &search_results](const auto& element)
		{
			SearchResult<String> search_result;
			search_result.m_String = get_string_func(element);
			search_result.m_PatternMatch = FuzzyMatch(input_pattern, FuzzySearchStringRef<String>(search_result.m_String), search_config);

			if (search_result.m_PatternMatch.m_Score > 0)
			{
				search_results.push_back(std::move(search_result));
			}
		});

		std::sort(search_results.begin(), search_results.end(), [](const SearchResult<String>& lhs, const SearchResult<String>& rhs) noexcept
		{
			if (lhs.m_PatternMatch.m_Score > rhs.m_PatternMatch.m_Score)
			{
				return true;
			}
			else if (lhs.m_PatternMatch.m_Score == rhs.m_PatternMatch.m_Score)
			{
				return FuzzySearchStringRef<String>(lhs.m_String).Length() < FuzzySearchStringRef<String>(rhs.m_String).Length();
			}
			return false;
		});

		return search_results;
	}

} // namespace NFuzzySearch
