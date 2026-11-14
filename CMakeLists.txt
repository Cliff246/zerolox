
cmake_minimum_required(VERSION 3.15)

project(zerolox C)



add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    add_executable(MY_lexer_test tests/lexer_test.c)
    target_link_libraries(MY_lexer_test PRIVATE MY_library)
    add_test(NAME lexer COMMAND MY_lexer_test)
endif()


add_custom_target(copy_asm ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_CURRENT_SOURCE_DIR}/asm
			${CMAKE_BINARY_DIR}/asm
	COMMENT "Copying asm directory to build"
)
