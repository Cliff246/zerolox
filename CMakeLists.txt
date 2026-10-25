
cmake_minimum_required(VERSION 3.15)

project(zerolox C)



add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    add_executable(MY_lexer_test tests/lexer_test.c)
    target_link_libraries(MY_lexer_test PRIVATE MY_library)
    add_test(NAME lexer COMMAND MY_lexer_test)
endif()
