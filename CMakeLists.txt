cmake_minimum_required(VERSION 3.20)
project(CVid)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("include")
include_directories("ext")

file(GLOB_RECURSE SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "./include/cvid/*.h")

add_library(CVid ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(CVid PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(CVid engine)

add_subdirectory("sandboxes")
add_subdirectory("app")

file(COPY "app/ConsoleWindowApp.exe" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/sandbox1)
file(COPY "app/ConsoleWindowApp.exe" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/sandbox2)
file(COPY "app/ConsoleWindowApp.exe" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/sandbox3)
file(COPY "app/ConsoleWindowApp.exe" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/sandboxes/sandbox4)