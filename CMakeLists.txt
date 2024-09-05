cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_NAME "CyclingCalculator_PowerEstimate")
project(${PROJECT_NAME})

find_package(CURL REQUIRED)

file(GLOB_RECURSE SRC_FILES "src/*.cpp" )
file(GLOB_RECURSE HEADER_FILES "src/*.h" )

include_directories(external)
add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

if (UNIX AND NOT APPLE)
    target_compile_options(${PROJECT_NAME} PRIVATE -O3 -march=native -mtune=native -mavx2 -mfma -g)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _LINUX)
elseif (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32)
endif()

target_link_libraries(${PROJECT_NAME} CURL::libcurl)

# cmake -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake ..
