cmake_minimum_required (VERSION 2.8)

project (Prt)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(LINUX ON)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(MACOSX ON)
endif()

if (NOT Win32)
	find_package(Threads REQUIRED)
	if(THREADS_HAVE_PTHREAD_ARG)
	  set_property(TARGET Prt PROPERTY COMPILE_OPTIONS "-pthread")
	  set_property(TARGET Prt PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
	endif()
	if(CMAKE_THREAD_LIBS_INIT)
	  target_link_libraries(Prt "${CMAKE_THREAD_LIBS_INIT}")
	endif()
endif()

if (NOT Win32)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	  add_definitions( -DPRT_USE_CLANG)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	  add_definitions( -DPRT_USE_GCC )
	endif()
endif()

set ( Dependencies_Path ${PROJECT_SOURCE_DIR}/../Dependencies/)
set ( P_Path ${Dependencies_Path}/P/ )
set ( P_Prt_Src_Path ${P_Path}/Src/Prt/)

if(Win32)
    add_definitions( -DPRT_PLAT_WINUSER )
elseif(LINUX OR MACOSX)
    add_definitions( -DPRT_PLAT_LINUXUSER )
endif()

macro ( Publish_Library_Header target )
	set (extra_macro_args ${ARGN})
	list(LENGTH extra_macro_args num_extra_args)
	if(${num_extra_args} EQUAL 0)
		get_property(Published_Headers_PATHS TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
	else()
		set(Published_Headers_PATHS "${ARGN}")
	endif()
	add_custom_command(TARGET ${target} POST_BUILD
	  COMMENT "Moving header files to Bld/include/"
	  COMMAND ${CMAKE_COMMAND} ARGS -E
	    make_directory ${LIBRARY_OUTPUT_INCLUDE_PATH}
	)

	foreach(incl_file_path ${Published_Headers_PATHS})
		file ( GLOB incl_files ${incl_file_path}/*.h )
		foreach(incl_file ${incl_files})
			add_custom_command(TARGET ${target} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different 
					${incl_file}
			       	${LIBRARY_OUTPUT_INCLUDE_PATH}
			)
		endforeach()
	endforeach()
endmacro()

add_subdirectory ( Prt )
add_subdirectory ( PrtTest )
