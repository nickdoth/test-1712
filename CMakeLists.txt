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
    # GIT_REPOSITORY https://github.com/nickdoth/libui
    # GIT_TAG origin/cmake-install
    PREFIX ${EXTERNAL_INSTALL_LOCATION}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)
add_dependencies(test-1712 dep_libui)
#find_library(EXT_UI NAME ui PATHS ${EXTERNAL_INSTALL_LOCATION}/libui/lib)
add_library(ui SHARED IMPORTED)
set_target_properties(ui PROPERTIES IMPORTED_LOCATION ${EXTERNAL_INSTALL_LOCATION}/lib/libui.dylib)

# Deps: libportaudio
ExternalProject_Add(
    dep_portaudio
    # GIT_REPOSITORY https://git.assembla.com/portaudio.git
    PREFIX ${EXTERNAL_INSTALL_LOCATION}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)
add_dependencies(test-1712 dep_portaudio)
#find_library(EXT_PORTAUDIO NAME portaudio PATHS ${EXTERNAL_INSTALL_LOCATION}/portaudio/lib)
add_library(portaudio SHARED IMPORTED)
set_target_properties(portaudio PROPERTIES IMPORTED_LOCATION ${EXTERNAL_INSTALL_LOCATION}/lib/libportaudio.dylib)

include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
target_link_libraries(test-1712 portaudio ui)
