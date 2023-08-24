#include <catch2/catch.hpp>

#include <FuzzySearch.h>

using namespace FuzzySearch;

static const std::string& GetStringFunc(const std::string& string)
{
	return string;
}

TEST_CASE("SourceFiles")
{
	std::vector<std::string> files = {
	    "e:/libs/nodehierarchy/main/source/BaseEntityNode.cpp",
	    "e:/libs/nodehierarchy/main/source/BaseEntityNode.h",
	    "e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.cpp",
	    "e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.h",
	    "e:/libs/nodehierarchy/main/source/BaseHierarchyNode.cpp",
	    "e:/libs/nodehierarchy/main/source/BaseHierarchyNode.h",
	    "e:/libs/nodehierarchy/main/source/BaseObjectNode.cpp",
	    "e:/libs/nodehierarchy/main/source/BaseObjectNode.h",
	    "e:/libs/nodehierarchy/main/source/CMakeLists.txt",
	    "e:/libs/otherlib/main/source/CMakeLists.txt",
	    "e:/libs/otherlib/main/source/no_extension",
	};

	SearchConfig config;
	config.m_MatchMode = MatchMode::E_SOURCE_FILES;
	config.m_MaxUnmatchedCharactersFromPattern = 2;

	SECTION("search string = bhn")
	{
		std::vector<SearchResult<std::string>> results = Search(std::string("bhn"), files.begin(), files.end(), &GetStringFunc, config);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNode.cpp" == results[0].m_String);
		REQUIRE(std::vector({34, 38, 47}) == results[0].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNode.h" == results[1].m_String);
		REQUIRE(std::vector({34, 38, 47}) == results[1].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.cpp" == results[2].m_String);
		REQUIRE(std::vector({34, 38, 47}) == results[2].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.h" == results[3].m_String);
		REQUIRE(std::vector({34, 38, 47}) == results[3].m_PatternMatch.m_Matches);
	}

	SECTION("search string = bhnl")
	{
		std::vector<SearchResult<std::string>> results = Search(std::string("bhnl"), files.begin(), files.end(), &GetStringFunc, config);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.cpp" == results[0].m_String);
		REQUIRE(std::vector({34, 38, 47, 51}) == results[0].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.h" == results[1].m_String);
		REQUIRE(std::vector({34, 38, 47, 51}) == results[1].m_PatternMatch.m_Matches);
	}

	SECTION("search string = hierarchy node base")
	{
		std::vector<SearchResult<std::string>> results = Search(std::string("hierarchy node base"), files.begin(), files.end(), &GetStringFunc, config);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNode.cpp" == results[0].m_String);
		REQUIRE(std::vector({38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 34, 35, 36, 37}) == results[0].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNode.h" == results[1].m_String);
		REQUIRE(std::vector({38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 34, 35, 36, 37}) == results[1].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.cpp" == results[2].m_String);
		REQUIRE(std::vector({38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 34, 35, 36, 37}) == results[2].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.h" == results[3].m_String);
		REQUIRE(std::vector({38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 34, 35, 36, 37}) == results[3].m_PatternMatch.m_Matches);
	}

	SECTION("search string = cmakelists node")
	{
		std::vector<SearchResult<std::string>> results = Search(std::string("cmakelists node"), files.begin(), files.end(), &GetStringFunc, config);
		REQUIRE("e:/libs/nodehierarchy/main/source/CMakeLists.txt" == results[0].m_String);
		REQUIRE(std::vector({34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 8, 9, 10, 11}) == results[0].m_PatternMatch.m_Matches);
	}
}

TEST_CASE("Filenames")
{
	std::vector<std::string> files = {
	    "e:/libs/nodehierarchy/main/source/BaseEntityNode.cpp",
	    "e:/libs/nodehierarchy/main/source/BaseEntityNode.h",
	    "e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.cpp",
	    "e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.h",
	    "e:/libs/nodehierarchy/main/source/BaseHierarchyNode.cpp",
	    "e:/libs/nodehierarchy/main/source/BaseHierarchyNode.h",
	    "e:/libs/nodehierarchy/main/source/BaseObjectNode.cpp",
	    "e:/libs/nodehierarchy/main/source/BaseObjectNode.h",
	    "e:/libs/nodehierarchy/main/source/CMakeLists.txt",
	    "e:/libs/otherlib/main/source/CMakeLists.txt",
	    "e:/libs/otherlib/main/source/no_extension",
	};

	SearchConfig config;
	config.m_MatchMode = MatchMode::E_FILENAMES;
	config.m_MaxUnmatchedCharactersFromPattern = 2;

	SECTION("search string = bhn")
	{
		std::vector<SearchResult<std::string>> results = Search(std::string("bhn"), files.begin(), files.end(), &GetStringFunc, config);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNode.h" == results[0].m_String);
		REQUIRE(std::vector({34, 38, 47}) == results[0].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNode.cpp" == results[1].m_String);
		REQUIRE(std::vector({34, 38, 47}) == results[1].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.h" == results[2].m_String);
		REQUIRE(std::vector({34, 38, 47}) == results[2].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.cpp" == results[3].m_String);
		REQUIRE(std::vector({34, 38, 47}) == results[3].m_PatternMatch.m_Matches);
	}

	SECTION("search string = bhnl")
	{
		std::vector<SearchResult<std::string>> results = Search(std::string("node loader"), files.begin(), files.end(), &GetStringFunc, config);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.h" == results[0].m_String);
		REQUIRE(std::vector({47, 48, 49, 50, 51, 52, 53, 54, 55, 56}) == results[0].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.cpp" == results[1].m_String);
		REQUIRE(std::vector({47, 48, 49, 50, 51, 52, 53, 54, 55, 56}) == results[1].m_PatternMatch.m_Matches);
	}

	SECTION("search string = hierarchy node base")
	{
		std::vector<SearchResult<std::string>> results = Search(std::string("hierarchy node base"), files.begin(), files.end(), &GetStringFunc, config);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNode.h" == results[0].m_String);
		REQUIRE(std::vector({38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 34, 35, 36, 37}) == results[0].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNode.cpp" == results[1].m_String);
		REQUIRE(std::vector({38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 34, 35, 36, 37}) == results[1].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.h" == results[2].m_String);
		REQUIRE(std::vector({38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 34, 35, 36, 37}) == results[2].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/BaseHierarchyNodeLoader.cpp" == results[3].m_String);
		REQUIRE(std::vector({38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 34, 35, 36, 37}) == results[3].m_PatternMatch.m_Matches);
	}

	SECTION("search string = cmakelists node")
	{
		std::vector<SearchResult<std::string>> results = Search(std::string("cmakelists"), files.begin(), files.end(), &GetStringFunc, config);
		REQUIRE("e:/libs/otherlib/main/source/CMakeLists.txt" == results[0].m_String);
		REQUIRE(std::vector({29, 30, 31, 32, 33, 34, 35, 36, 37, 38}) == results[0].m_PatternMatch.m_Matches);
		REQUIRE("e:/libs/nodehierarchy/main/source/CMakeLists.txt" == results[1].m_String);
		REQUIRE(std::vector({34, 35, 36, 37, 38, 39, 40, 41, 42, 43}) == results[1].m_PatternMatch.m_Matches);
	}
}

TEST_CASE("Strings")
{
	std::vector<std::string> files = {
	    "git init",
	    "git status",
	    "git add my_new_file.txt",
	    "git commit -m \"Add three files\"",
	    "git reset --soft HEAD^",
	    "git remote add origin https://github.com/heftyy/fuzzy-search.git",
	};

	SearchConfig config;
	config.m_MatchMode = MatchMode::E_STRINGS;
	config.m_MaxUnmatchedCharactersFromPattern = 2;

	SECTION("search string = git")
	{
		std::vector<SearchResult<std::string>> results = Search(std::string("git"), files.begin(), files.end(), &GetStringFunc, config);
		REQUIRE("git init" == results[0].m_String);
		REQUIRE(std::vector({0, 1, 2}) == results[0].m_PatternMatch.m_Matches);
		REQUIRE("git status" == results[1].m_String);
		REQUIRE(std::vector({0, 1, 2}) == results[1].m_PatternMatch.m_Matches);
	}

	SECTION("search string = add")
	{
		std::vector<SearchResult<std::string>> results = Search(std::string("add"), files.begin(), files.end(), &GetStringFunc, config);
		REQUIRE("git add my_new_file.txt" == results[0].m_String);
		REQUIRE(std::vector({4, 5, 6}) == results[0].m_PatternMatch.m_Matches);
		REQUIRE("git commit -m \"Add three files\"" == results[1].m_String);
		REQUIRE(std::vector({15, 16, 17}) == results[1].m_PatternMatch.m_Matches);
		REQUIRE("git remote add origin https://github.com/heftyy/fuzzy-search.git" == results[2].m_String);
		REQUIRE(std::vector({11, 12, 13}) == results[2].m_PatternMatch.m_Matches);
	}

	SECTION("search string = reset")
	{
		std::vector<SearchResult<std::string>> results = Search(std::string("reset"), files.begin(), files.end(), &GetStringFunc, config);
		REQUIRE("git reset --soft HEAD^" == results[0].m_String);
		REQUIRE(std::vector({4, 5, 6, 7, 8}) == results[0].m_PatternMatch.m_Matches);
	}
}
