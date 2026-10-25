

set(MY_LIBRARY_CFILES ${MY_LIBRARY_CFILES}

	${CMAKE_CURRENT_SOURCE_DIR}/MY_commons.c
	${CMAKE_CURRENT_SOURCE_DIR}/MY_lexer.c
)

add_library(MY_library ${MY_LIBRARY_CFILES})


include_directories(MY_library)

target_link_libraries(MY_library
	PUBLIC
)

target_include_directories(MY_library
	PUBLIC

	${CMAKE_CURRENT_SOURCE_DIR}/
)
