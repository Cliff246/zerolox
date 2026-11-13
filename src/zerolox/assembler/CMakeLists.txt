set(ASM_CFILE ${ASM_CFILE}

	${CMAKE_CURRENT_SOURCE_DIR}/ASM_main.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Assembler.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_syntax.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Argument.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Statement.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Form.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Handler.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_Operation.c
	${CMAKE_CURRENT_SOURCE_DIR}/ASM_commons.c
	
)

add_executable(zlx-asm ${ASM_CFILE})

include_directories(zlx-asm)

target_link_libraries(zlx-asm 
	PUBLIC

	ZLX_commons
	MY_library
)

target_include_directories(zlx-asm
	PUBLIC

	ZLX_commons
	MY_library
)

