cmake_minimum_required(VERSION 3.20)
project(CS2DS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(FLTK REQUIRED)
find_package(PNG QUIET)
find_package(JPEG QUIET)
find_package(ZLIB QUIET)

find_library(FLTK_IMAGES_LIB
    NAMES fltk_images
    HINTS ${FLTK_LIBRARY_DIRS}
          /opt/homebrew/lib
          /usr/local/lib
          /usr/lib
          /usr/lib/x86_64-linux-gnu
)

if(NOT FLTK_IMAGES_LIB)
    message(FATAL_ERROR "FLTK images library (fltk_images) not found. Install FLTK with image support (e.g. brew install fltk).")
endif()

file(GLOB_RECURSE MODEL_SRC CONFIGURE_DEPENDS src/model/*.cpp)
file(GLOB_RECURSE VIEW_SRC CONFIGURE_DEPENDS src/view/*.cpp)
file(GLOB_RECURSE CTRL_SRC CONFIGURE_DEPENDS src/controller/*.cpp)
file(GLOB_RECURSE COMMON_SRC CONFIGURE_DEPENDS src/common/*.cpp)

add_executable(cs2ds
    src/main.cpp
    ${MODEL_SRC}
    ${VIEW_SRC}
    ${CTRL_SRC}
    ${COMMON_SRC}
)

target_include_directories(cs2ds PRIVATE src ${FLTK_INCLUDE_DIR})
target_link_libraries(cs2ds PRIVATE
    ${FLTK_IMAGES_LIB}
    ${FLTK_LIBRARIES}
    pthread
)

if(PNG_FOUND)
    target_link_libraries(cs2ds PRIVATE PNG::PNG)
endif()
if(JPEG_FOUND)
    target_link_libraries(cs2ds PRIVATE JPEG::JPEG)
endif()
if(ZLIB_FOUND)
    target_link_libraries(cs2ds PRIVATE ZLIB::ZLIB)
endif()

add_custom_command(
    TARGET cs2ds
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying assets to build directory"
)
