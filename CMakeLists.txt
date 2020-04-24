CMAKE_MINIMUM_REQUIRED(VERSION 3.13.1)

SET(TARGET "CA")
PROJECT(${TARGET} VERSION 1.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(pybind11)
set(PYBIND11_CPP_STANDARD -std=c++14)

set(JSON_BuildTests OFF CACHE INTERNAL "")
include(FetchContent)
FetchContent_Declare(
  nlohmann
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        develop
)
FetchContent_MakeAvailable(nlohmann)

add_library(${TARGET} MODULE ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(${TARGET} PRIVATE pybind11::module)
target_link_libraries(${TARGET} PRIVATE nlohmann_json::nlohmann_json)
set_target_properties(${TARGET} PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")
# pybind11_add_module(${CA} ${TARGET_SRC})

