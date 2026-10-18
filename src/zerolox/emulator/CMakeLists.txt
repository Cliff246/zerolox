
set(EMU_CFILE ${EMU_CFILE}
	${CMAKE_CURRENT_SOURCE_DIR}/EMU_zlx.c
	${CMAKE_CURRENT_SOURCE_DIR}/EMU_Context.c
	${CMAKE_CURRENT_SOURCE_DIR}/EMU__CPU.c
	${CMAKE_CURRENT_SOURCE_DIR}/EMU_Machine.c
	${CMAKE_CURRENT_SOURCE_DIR}/EMU_Memory.c
)

add_executable(ZLX_emu ${EMU_CFILE})

include_directories(ZLX_emu)

target_link_libraries(ZLX_emu 
	PUBLIC

	ZLX_commons
	MY_library
)

target_include_directories(ZLX_emu
	PUBLIC

	ZLX_commons
	MY_library
)
