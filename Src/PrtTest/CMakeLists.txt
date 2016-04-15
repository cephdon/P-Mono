set ( Core_Path ${CMAKE_CURRENT_SOURCE_DIR}/Core/ )
file ( GLOB Core_Src ${Core_Path}/*.cpp ${Core_Path}/*.c ${Core_Path}/*.h )

set ( PrtTestSrc ${Core_Src} )

set( PrtTest_Output_PATH ${PROJECT_SOURCE_DIR}/../Bld/Drops/PrtTest/)
set( LIBRARY_OUTPUT_PATH ${PrtTest_Output_PATH}/lib/ )
set( LIBRARY_OUTPUT_INCLUDE_PATH ${PrtTest_Output_PATH}/include/ )

add_definitions( -DPRT_TEST )

add_library(PrtTest SHARED ${PrtTestSrc} $<TARGET_OBJECTS:Prt_test>)
get_property(Prt_Published_Headers_PATHS TARGET Prt PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(PrtTest PUBLIC ${Prt_Published_Headers_PATHS})
target_include_directories(PrtTest PUBLIC ${Core_Path})

Publish_Library_Header(PrtTest ${Core_Path})
