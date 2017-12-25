cmake_minimum_required (VERSION 2.6)
project (Test1712)
find_package(Git REQUIRED)
include(ExternalProject)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

# Common complie options
add_compile_options(-std=c++11)

# Built dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/out")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/out")
set(CMAKE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/out")

# main
add_executable(test-1712 main.cpp)

# Deps: libui
ExternalProject_Add(
    dep_libui
    GIT_REPOSITORY https://github.com/nickdoth/libui
    GIT_TAG origin/cmake-install
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)
add_dependencies(test-1712 dep_libui)
find_library(EXT_UI NAME ui PATHS ${EXTERNAL_INSTALL_LOCATION}/lib)

# Deps: libportaudio
ExternalProject_Add(
    dep_portaudio
    GIT_REPOSITORY https://git.assembla.com/portaudio.git
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)
add_dependencies(test-1712 dep_portaudio)
find_library(EXT_PORTAUDIO NAME portaudio PATHS ${EXTERNAL_INSTALL_LOCATION}/lib)

include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
target_link_libraries(test-1712 ${EXT_UI} ${EXT_PORTAUDIO})