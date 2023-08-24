#include <algorithm>
#include <cctype>
#include <regex>

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch.hpp>

#include <Files.h>
#include <FuzzySearch.h>

std::vector<std::string> StringSearch(const std::vector<std::string>& split_by_space, const std::vector<std::string>& files)
{
	std::vector<std::string> results;
	results.reserve(files.size());

	for (std::string file : files)
	{
		std::transform(file.begin(), file.end(), file.begin(), [](unsigned char c) { return std::tolower(c); });

		size_t found_count = 0;
		for (const std::string& str : split_by_space)
		{
			const auto found = file.find_last_of(str);
			if (found != std::string::npos)
			{
				++found_count;
			}
			else
			{
				break;
			}
		}

		if (found_count == split_by_space.size())
		{
			results.push_back(file);
		}
	}

	return results;
}

std::vector<std::string> RegexSearch(const std::vector<std::string>& split_by_space, const std::vector<std::string>& files)
{
	std::vector<std::string> results;
	results.reserve(files.size());
	for (const std::string& str : split_by_space)
	{
		std::regex self_regex(str, std::regex_constants::icase);
		for (const std::string& file : files)
		{
			if (std::regex_search(file, self_regex))
			{
				results.push_back(file);
			}
		}
	}
	return results;
}

std::vector<std::string> BoyerMoore(const std::vector<std::string>& split_by_space, const std::vector<std::string>& files)
{
	std::vector<std::string> results;
	results.reserve(files.size());
	for (const std::string& str : split_by_space)
	{
		for (const std::string& file : files)
		{
			auto it = std::search(file.cbegin(), file.cend(), std::boyer_moore_horspool_searcher(str.begin(), str.end()));
			if (it != file.end())
			{
				results.push_back(file);
			}
		}
	}
	return results;
}

const char* GetStringFunc(const std::string& string)
{
	return string.c_str();
}

TEST_CASE("Fuzzy search benchmark")
{
	std::vector<std::string> files;
	for (const char* str : FuzzySearchBenchmark::FILES)
	{
		files.emplace_back(str);
	}

	std::vector<std::string> split_by_space_long = {"qt", "base", "view", "list"};
	std::vector<std::string> split_by_space_short = {"TABLE"};

	FuzzySearch::SearchConfig config;
	config.m_MatchMode = FuzzySearch::MatchMode::E_SOURCE_FILES;
	config.m_MaxUnmatchedCharactersFromPattern = 2;

	BENCHMARK("FuzzyLongPattern") { return FuzzySearch::Search<const char*>("qt base view list", files.begin(), files.end(), &GetStringFunc, config); };

	BENCHMARK("FuzzyShortPattern") { return FuzzySearch::Search<const char*>("TABLE", files.begin(), files.end(), &GetStringFunc, config); };

	BENCHMARK("BoyerMooreLongPattern") { return BoyerMoore(split_by_space_long, files); };

	BENCHMARK("BoyerMooreShortPattern") { return BoyerMoore(split_by_space_short, files); };

	BENCHMARK("FindLongPattern") { return StringSearch(split_by_space_long, files); };

	BENCHMARK("FindShortPattern") { return StringSearch(split_by_space_short, files); };

	BENCHMARK("RegexLongPattern") { return RegexSearch(split_by_space_long, files); };

	BENCHMARK("RegexShortPattern") { return RegexSearch(split_by_space_short, files); };
}
