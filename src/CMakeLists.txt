# CMAKE_MINIMUM_REQUIRED(VERSION 3.13.1)
# for testing
set(HEADER_LIST "${CMAKE_SOURCE_DIR}/include/CA/agent.h"
	"${CMAKE_SOURCE_DIR}/include/CA/mesh.h"
	"${CMAKE_SOURCE_DIR}/include/CA/frame.h"
	"${CMAKE_SOURCE_DIR}/include/CA/patch.h"
	"${CMAKE_SOURCE_DIR}/include/CA/settings.h"
	"${CMAKE_SOURCE_DIR}/include/CA/toolbox.h")



set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
# include(cotire)

pybind11_add_module(${TARGET} model.cpp ${HEADER_LIST})
# cotire(${TARGET})
target_link_libraries(${TARGET} PRIVATE pybind11::module)
target_link_libraries(${TARGET} PRIVATE fmt::fmt)
target_include_directories(${TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/include)


source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})
