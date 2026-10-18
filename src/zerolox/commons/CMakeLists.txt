
set(ZLX_COMMONS_CFILES ${ZLX_COMMONS_CFILES}
	${CMAKE_CURRENT_SOURCE_DIR}/isa/ISA_zerolox.c
)


add_library(ZLX_commons ${ZLX_COMMONS_CFILES})


include_directories(ZLX_commons)

target_include_directories(ZLX_commons 
	PUBLIC

	${CMAKE_CURRENT_SOURCE_DIR}/
)

