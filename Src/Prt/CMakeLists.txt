cmake_minimum_required (VERSION 2.6)

project (Prt)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(LINUX ON)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(MACOSX ON)
endif()

set ( Dependencies_Path ${PROJECT_SOURCE_DIR}/../../Dependencies/)
set ( P_Path ${Dependencies_Path}/P/ )
set ( Prt_Src_Path ${P_Path}/Src/Prt/)

set ( API_PATH ${Prt_Src_Path}/API/ )
set ( Core_PATH ${Prt_Src_Path}/Core/ )
if(Win32)
	set ( User_PATH ${Prt_Src_Path}/WinUser/ )
    add_definitions( -DPRT_PLAT_WINUSER )
elseif(LINUX OR MACOSX)
	set ( User_PATH ${CMAKE_CURRENT_SOURCE_DIR}/LinuxUser/ )
    add_definitions( -DPRT_PLAT_LINUXUSER )
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
	  add_definitions( -DFORCEINLINE= )
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	  add_definitions( -DFORCEINLINE=__attribute__((always_inline)) )
	endif()
endif()

set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../Bld/Drops/PrtUser/ )
set( LIBRARY_OUTPUT_INCLUDE_PATH ${LIBRARY_OUTPUT_PATH}/include/ )

file ( GLOB API_SRC ${API_PATH}/*.c ${API_PATH}/*.h )
file ( GLOB Core_SRC ${Core_PATH}/*.c ${Core_PATH}/*.h )
file ( GLOB User_SRC ${User_PATH}/*.c ${User_PATH}/*.h )

set ( PrtUserSrc ${API_SRC} ${Core_SRC} ${User_SRC} )
add_library ( Prt SHARED ${PrtUserSrc} )

target_include_directories(Prt PUBLIC ${API_PATH})
target_include_directories(Prt PUBLIC ${Core_PATH})
target_include_directories(Prt PUBLIC ${User_PATH})

add_custom_command(TARGET Prt POST_BUILD
  COMMENT "Moving header files to Bld/include/"
  COMMAND ${CMAKE_COMMAND} ARGS -E
    make_directory ${LIBRARY_OUTPUT_INCLUDE_PATH}
)

get_property(Prt_Published_Headers_PATHS TARGET Prt PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
foreach(incl_file_path ${Prt_Published_Headers_PATHS})
	file ( GLOB incl_files ${incl_file_path}/*.h )
	foreach(incl_file ${incl_files})
		add_custom_command(TARGET Prt POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different 
				${incl_file}
		       	${LIBRARY_OUTPUT_INCLUDE_PATH}
		)
	endforeach()
endforeach()
