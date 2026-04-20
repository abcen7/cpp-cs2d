cmake_minimum_required(VERSION 3.20)
project(CS2DS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(FLTK REQUIRED)

file(GLOB_RECURSE MODEL_SRC CONFIGURE_DEPENDS src/model/*.cpp)
file(GLOB_RECURSE VIEW_SRC CONFIGURE_DEPENDS src/view/*.cpp)
file(GLOB_RECURSE CTRL_SRC CONFIGURE_DEPENDS src/controller/*.cpp)

add_executable(cs2ds
    src/main.cpp
    ${MODEL_SRC}
    ${VIEW_SRC}
    ${CTRL_SRC}
)

target_include_directories(cs2ds PRIVATE src ${FLTK_INCLUDE_DIR})
target_link_libraries(cs2ds PRIVATE ${FLTK_LIBRARIES} pthread)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
