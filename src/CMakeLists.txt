set(FUZZY_SEARCH_SOURCE_FILES
        FuzzySearch.inl
        FuzzySearch.h
        )

add_library(fuzzy_search_lib INTERFACE ${FUZZY_SEARCH_SOURCE_FILES})
target_compile_options(fuzzy_search_lib INTERFACE ${COMPILE_FLAGS})
target_compile_features(fuzzy_search_lib INTERFACE cxx_std_17)
target_include_directories(fuzzy_search_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET fuzzy_search_lib PROPERTY CXX_CLANG_TIDY ${CLANG_TIDY_ARGS})
