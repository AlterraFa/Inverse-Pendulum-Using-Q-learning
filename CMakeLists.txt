cmake_minimum_required(VERSION 3.28)
project(PendulumSimulation)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)


find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
message(STATUS "Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
include_directories(${PYTHON_INCLUDE_DIRS})
find_package(pybind11 CONFIG REQUIRED)

add_executable(PendulumExec
        src/main.cpp
        src/events.cpp
        src/layout.cpp
        src/obj.cpp
)

set_target_properties(sfml-system sfml-window sfml-graphics sfml-network sfml-audio PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_library(events STATIC src/events.cpp)
set_target_properties(events PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_library(layout STATIC src/layout.cpp)
set_target_properties(layout PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_library(object STATIC src/obj.cpp)
set_target_properties(object PROPERTIES POSITION_INDEPENDENT_CODE ON)


pybind11_add_module(PendulumSimulation
        src/bindings.cpp
        src/events.cpp
        src/layout.cpp
        src/obj.cpp
)

target_link_libraries(PendulumSimulation PRIVATE pybind11::module sfml-window)
target_link_libraries(PendulumExec PRIVATE sfml-graphics)
target_compile_features(PendulumExec PRIVATE cxx_std_11)
target_link_libraries(PendulumSimulation PRIVATE sfml-graphics)
target_link_libraries(PendulumSimulation PRIVATE events)
target_link_libraries(PendulumSimulation PRIVATE layout)
target_link_libraries(PendulumSimulation PRIVATE object)

if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:PendulumExec>
        VERBATIM)
endif()