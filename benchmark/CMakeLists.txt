find_package(Catch2 CONFIG REQUIRED)

add_executable(fuzzy_search_benchmark FuzzySearchBenchmark.cpp Files.h)
target_compile_features(fuzzy_search_benchmark PUBLIC cxx_std_17)
target_link_libraries(fuzzy_search_benchmark PRIVATE fuzzy_search_lib Catch2::Catch2WithMain)
