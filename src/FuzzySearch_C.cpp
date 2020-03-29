#include "FuzzySearch_C.h"
#include "FuzzySearch.h"

FuzzySearchInputPattern* CreatePattern(const char* pattern)
{
    return new FuzzySearch::InputPattern(pattern);
}

FuzzySearchPatternMatch FuzzyMatch(FuzzySearchInputPattern* input_pattern, const char* str, FuzzySearchMatchMode match_mode)
{
    FuzzySearch::MatchMode match_mode_enum = (FuzzySearch::MatchMode) match_mode;
    FuzzySearch::InputPattern* pattern = (FuzzySearch::InputPattern*)input_pattern;
    FuzzySearch::PatternMatch pattern_match = FuzzySearch::FuzzyMatch(*pattern, str, match_mode_enum);

    const int size = pattern_match.m_Matches.size();
    FuzzySearchPatternMatch result;
    result.m_Score = pattern_match.m_Score;
    result.m_MatchesSize = size;
    result.m_Matches = new int[size];
    memcpy(result.m_Matches, pattern_match.m_Matches.data(), size);
    return result;
}