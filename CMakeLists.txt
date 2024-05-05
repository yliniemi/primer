cmake_minimum_required(VERSION 3.10)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_NAME ${PROJECT_NAME})
project(${PROJECT_NAME} C)

# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS OFF)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
link_libraries(m pthread)

add_executable(${PROJECT_NAME} src/main.c)

# install(TARGETS ${PROJECT_NAME} DESTINATION bin)
