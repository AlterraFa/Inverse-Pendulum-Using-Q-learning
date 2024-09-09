cmake_minimum_required(VERSION 3.28)
project(InvertedPendulum LANGUAGES CXX)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)


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

add_executable(InvertedPendulum 
        src/main.cpp
        src/events.cpp
        src/layout.cpp
        src/obj.cpp
        src/physics.cpp
        # src/test.cpp
        )

target_link_libraries(InvertedPendulum PRIVATE sfml-graphics)
target_link_libraries(InvertedPendulum PRIVATE pybind11::embed)
target_compile_features(InvertedPendulum PRIVATE cxx_std_11)

if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()
