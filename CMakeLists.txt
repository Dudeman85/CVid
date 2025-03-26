cmake_minimum_required(VERSION 3.20)
project(CVid)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CVID_BUILD_DEMOS "" ON)

include_directories("include")
include_directories("ext")
add_subdirectory("app")

file(GLOB_RECURSE SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "./include/cvid/*.h")
add_library(CVid ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(CVid PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(CVID_BUILD_DEMOS)
    add_subdirectory("demos")
    file(COPY "app/ConsoleWindowApp.exe" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/demos/demo1)
    file(COPY "app/ConsoleWindowApp.exe" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/demos/demo2)
endif()