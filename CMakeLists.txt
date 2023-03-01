cmake_minimum_required(VERSION 3.16.3)

project(test LANGUAGES CUDA CXX)

enable_language(CUDA)

include(CheckLanguage)
check_language(CXX)
check_language(CUDA)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)


set(TARGET "traits")
add_executable(${TARGET})
target_sources(${TARGET} PRIVATE traits/test.cpp)
target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/traits)
target_compile_options(${TARGET} PRIVATE -Werror -Wall)
target_link_libraries(${TARGET} PRIVATE -Wl,-rpath=/home/utils/gcc-7.3.0/lib64)


set(TARGET "bridge")
add_executable(${TARGET})
target_sources(${TARGET} PRIVATE bridge/test.cpp)
target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/bridge)
target_compile_options(${TARGET} PRIVATE -Werror -Wall)
target_link_libraries(${TARGET} PRIVATE -Wl,-rpath=/home/utils/gcc-7.3.0/lib64)
