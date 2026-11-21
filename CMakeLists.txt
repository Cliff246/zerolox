
cmake_minimum_required(VERSION 3.15)

project(zerolox C)



add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    add_executable(ZLX_ASM_argument_test
        tests/argument_test.c
        src/zerolox/assembler/ASM_Argument.c
    )
    target_include_directories(ZLX_ASM_argument_test PRIVATE src/zerolox/assembler)
    target_link_libraries(ZLX_ASM_argument_test PRIVATE MY_library)
    add_test(NAME assembler_argument COMMAND ZLX_ASM_argument_test)
    foreach(case invalid_head negative_head inactive_head null_group duplicate_root full)
        add_test(NAME assembler_argument_${case} COMMAND ZLX_ASM_argument_test ${case})
        set_tests_properties(assembler_argument_${case} PROPERTIES WILL_FAIL TRUE)
    endforeach()
    add_executable(MY_lexer_test tests/lexer_test.c)
    target_link_libraries(MY_lexer_test PRIVATE MY_library)
    add_test(NAME lexer COMMAND MY_lexer_test)
    add_executable(MY_token_group_test tests/token_group_test.c)
    target_link_libraries(MY_token_group_test PRIVATE MY_library)
    add_test(NAME token_group COMMAND MY_token_group_test)
    add_executable(ZLX_ASM_statement_test
        tests/statement_test.c
        src/zerolox/assembler/ASM_Statement.c
        src/zerolox/assembler/ASM_Operation.c
    )
    target_include_directories(ZLX_ASM_statement_test PRIVATE src/zerolox/assembler)
    target_link_libraries(ZLX_ASM_statement_test PRIVATE ZLX_commons MY_library)
    add_test(NAME assembler_statement COMMAND ZLX_ASM_statement_test)
endif()


add_custom_target(copy_asm ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_CURRENT_SOURCE_DIR}/asm
			${CMAKE_BINARY_DIR}/asm
	COMMENT "Copying asm directory to build"
)
