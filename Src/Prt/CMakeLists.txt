set ( API_PATH ${P_Prt_Src_Path}/API/ )
set ( Core_PATH ${P_Prt_Src_Path}/Core/ )

if(Win32)
	set ( User_PATH ${P_Prt_Src_Path}/WinUser/ )
elseif(LINUX OR MACOSX)
	set ( User_PATH ${CMAKE_CURRENT_SOURCE_DIR}/LinuxUser/ )
endif()

set( PrtUser_Output_PATH ${PROJECT_SOURCE_DIR}/../../Bld/Drops/PrtUser/)
set( LIBRARY_OUTPUT_PATH ${PrtUser_Output_PATH}/lib/ )
set( LIBRARY_OUTPUT_INCLUDE_PATH ${PrtUser_Output_PATH}/include/ )

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
