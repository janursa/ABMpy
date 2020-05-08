CMAKE_MINIMUM_REQUIRED(VERSION 3.13.1)
# for testing
SET(TESTS 
    "test_main.cpp"
    "test_CA.cpp")

include(FetchContent)
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(catch)
include_directories(${catch_SOURCE_DIR}/include)

add_executable("${PROJECT_NAME}_test" ${TESTS})
target_include_directories("${PROJECT_NAME}_test" PUBLIC ${CMAKE_SOURCE_DIR}/include)
# target_include_directories("${PROJECT_NAME}_test"  PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features("${PROJECT_NAME}_test" PRIVATE cxx_std_17)
target_link_libraries("${PROJECT_NAME}_test" PRIVATE pybind11::module)

target_link_libraries("${PROJECT_NAME}_test" PRIVATE Catch2::Catch2)
