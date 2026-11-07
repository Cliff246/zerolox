set(ASM_CFILE ${ASM_CFILE}

	${CMAKE_CURRENT_SOURCE_DIR}/ASM_main.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Assembler.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_syntax.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Argument.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Statement.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Form.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Handle.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Operation.c
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

