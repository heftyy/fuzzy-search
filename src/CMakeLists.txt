set(FUZZY_SEARCH_SOURCE_FILES
    FuzzySearch.cpp
    FuzzySearch.h
    )

add_library(fuzzy_search_lib ${FUZZY_SEARCH_SOURCE_FILES})
target_compile_options(fuzzy_search_lib PRIVATE ${COMPILE_FLAGS})
target_compile_features(fuzzy_search_lib PUBLIC cxx_std_17)
target_include_directories(fuzzy_search_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET fuzzy_search_lib PROPERTY CXX_CLANG_TIDY ${CLANG_TIDY_ARGS})

set(FUZZY_SEARCH_C_SOURCE_FILES
    FuzzySearch_C.cpp
    FuzzySearch_C.h
    )

add_library(fuzzy_search_c_lib SHARED ${FUZZY_SEARCH_C_SOURCE_FILES})
target_compile_features(fuzzy_search_c_lib PUBLIC cxx_std_17)
target_include_directories(fuzzy_search_c_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(fuzzy_search_c_lib PRIVATE fuzzy_search_lib)
if (MSVC)
    set_target_properties(fuzzy_search_c_lib PROPERTIES LINK_FLAGS "/fixed:no")
endif()
