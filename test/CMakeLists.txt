find_package(Catch2 CONFIG REQUIRED)

set(TEST_SRC_FILES
    TestFuzzySearch.cpp
)

add_executable(fuzzy_search_test ${TEST_SRC_FILES})
target_compile_features(fuzzy_search_test PRIVATE cxx_std_17)
target_link_libraries(fuzzy_search_test PRIVATE fuzzy_search_lib Catch2::Catch2WithMain)

add_test(NAME all_tests COMMAND $<TARGET_FILE:fuzzy_search_test>)
