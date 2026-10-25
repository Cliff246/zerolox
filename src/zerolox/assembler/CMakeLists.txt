set(ASM_CFILE ${ASM_CFILE}

	${CMAKE_CURRENT_SOURCE_DIR}/ASM_main.cc
)

add_executable(ZLX_asm ${ASM_CFILE})

include_directories(ZLX_asm)

target_link_libraries(ZLX_asm 
	PUBLIC

	ZLX_commons
	MY_library
)

target_include_directories(ZLX_asm
	PUBLIC

	ZLX_commons
	MY_library
)
