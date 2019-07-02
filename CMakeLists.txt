cmake_minimum_required(VERSION 3.2.3)

project(rest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -std=c++11 -fexceptions -pedantic -Wall -Wextra")

add_subdirectory(${PROJECT_SOURCE_DIR}/external/asio)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/kashmir)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/restbed)

add_definitions(-DASIO_STANDALONE)

include_directories(
    ${PROJECT_SOURCE_DIR}/external/asio/install/include
    ${PROJECT_SOURCE_DIR}/external/kashmir/install/include
    ${PROJECT_SOURCE_DIR}/external/restbed/install/include
    )

link_directories(
    ${PROJECT_SOURCE_DIR}/external/restbed/install/library
    )

file(GLOB_RECURSE VAR_SOURCES ${PROJECT_SOURCE_DIR}/src/test/*.cpp)
set(VAR_SOURCES ${VAR_SOURCES} ${PROJECT_SOURCE_DIR}/src/main.cpp)

add_executable(${PROJECT_NAME} ${VAR_SOURCES})
add_dependencies(${PROJECT_NAME} restbed_ext)
add_dependencies(restbed_ext kashmir_ext asio_ext)

target_link_libraries(${PROJECT_NAME} PUBLIC pthread stdc++ restbed ssl crypto)

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/install)
