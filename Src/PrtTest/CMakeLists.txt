set ( Core_Path ${CMAKE_CURRENT_SOURCE_DIR}/Core/ )
file ( GLOB Core_Src ${Core_Path}/*.c ${Core_Path}/*.h )

set ( PrtTestSrc ${Core_Src} )

set( PrtTest_Output_PATH ${PROJECT_SOURCE_DIR}/../Bld/Drops/PrtTest/)
set( LIBRARY_OUTPUT_PATH ${PrtTest_Output_PATH}/lib/ )
set( LIBRARY_OUTPUT_INCLUDE_PATH ${PrtTest_Output_PATH}/include/ )

add_library(PrtTest SHARED ${PrtTestSrc})

target_link_libraries(PrtTest PUBLIC Prt_static)

