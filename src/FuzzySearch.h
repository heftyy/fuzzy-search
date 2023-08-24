#pragma once

#include <vector>

namespace FuzzySearch
{
	template<typename String>
	class FuzzySearchStringRef
	{
	public:
		FuzzySearchStringRef()
		{
		}

		FuzzySearchStringRef(const String& str)
			: m_String(&str)
		{}

		inline int Length() const { static_assert(sizeof(String) == 0, "Create a specialization of FuzzySearchStringRef with the correct type"); return 0; }
		inline bool Empty() const { static_assert(sizeof(String) == 0, "Create a specialization of FuzzySearchStringRef with the correct type"); return 0; }

		inline bool Equals(int /*start*/, int /*length*/, const char* /*str*/) const { static_assert(sizeof(String) == 0, "Create a specialization of FuzzySearchStringRef with the correct type"); return 0; }
		inline int FindLastOf(const char* /*str*/) const { static_assert(sizeof(String) == 0, "Create a specialization of FuzzySearchStringRef with the correct type"); return 0; }

		inline bool IsLower(size_t /*index*/) const { static_assert(sizeof(String) == 0, "Create a specialization of FuzzySearchStringRef with the correct type"); return 0; }
		inline int ToLower(size_t /*index*/) const { static_assert(sizeof(String) == 0, "Create a specialization of FuzzySearchStringRef with the correct type"); return 0; }

		inline int operator[](size_t /*index*/) const { static_assert(sizeof(String) == 0, "Create a specialization of FuzzySearchStringRef with the correct type"); return 0; }

	private:
		const String* m_String{ nullptr };
	};

	enum class MatchMode : uint8_t
	{
		E_STRINGS,
		E_FILENAMES,
		E_SOURCE_FILES
	};

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
	template<typename String>
	struct InputPattern
	{
		InputPattern(String str)
		{
			SetString(str);
		}

		InputPattern& operator=(const InputPattern& lhs)
		{
			SetString(lhs.m_String);
			return *this;
		}

		void SetString(String str)
		{
			m_String = str;
			m_Pattern = { m_String };
			m_PatternMatches.resize(m_Pattern.Length());
			m_MatchIndexes.resize(m_Pattern.Length());
		}

		String m_String;
		FuzzySearchStringRef<String> m_Pattern;
		std::vector<PatternMatch> m_PatternMatches;
		std::vector<int> m_MatchIndexes;
	};

	struct SearchConfig
	{
		MatchMode m_MatchMode { MatchMode::E_STRINGS };
		uint8_t m_MaxUnmatchedCharactersFromPattern { 2 };
	};

	template<typename String>
	PatternMatch FuzzyMatch(InputPattern<String>& input_pattern, const FuzzySearchStringRef<String>& str, SearchConfig search_config);

	template<typename String>
	struct SearchResult
	{
		String m_String;
		PatternMatch m_PatternMatch;
	};

	template<typename String, typename Iterator, typename Func>
	std::vector<SearchResult<String>> Search(const String& pattern_str, Iterator begin, Iterator end, Func&& get_string_func, SearchConfig search_config);

} // namespace NFuzzySearch

#include "FuzzySearch.inl"
