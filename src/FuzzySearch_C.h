#ifndef FUZZY_SEARCH_C_H
#define FUZZY_SEARCH_C_H

typedef void FuzzySearchInputPattern;

struct FuzzySearchPatternMatch
{
	int m_Score;
    int* m_Matches;
    int m_MatchesSize;
};

enum FuzzySearchMatchMode
{
	E_FUZZY_SEARCH_STRINGS,
	E_FUZZY_SEARCH_FILENAMES,
	E_FUZZY_SEARCH_SOURCE_FILES
};

FuzzySearchInputPattern* CreatePattern(const char* pattern);
FuzzySearchPatternMatch FuzzyMatch(FuzzySearchInputPattern* input_pattern, const char* str, FuzzySearchMatchMode match_mode);

#endif /* FUZZY_SEARCH_C_H */