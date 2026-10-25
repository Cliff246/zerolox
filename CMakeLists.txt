
cmake_minimum_required(VERSION 3.15)

project(zerolox C)



add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    add_executable(ZLX_ASM_lexer_test tests/lexer_test.c src/zerolox/assembler/lexer.c)
    target_include_directories(ZLX_ASM_lexer_test PRIVATE src/zerolox/assembler)
    add_test(NAME lexer COMMAND ZLX_ASM_lexer_test)
endif()
