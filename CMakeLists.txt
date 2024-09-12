cmake_minimum_required(VERSION 3.28)
project(PendulumSimulation)
set(CMAKE_CXX_STANDARD 17)

# Set output directories for libraries and executables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Option to build shared libraries
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Include FetchContent for downloading SFML
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

# Add executable for the pendulum simulation
add_executable(PendulumExec
    src/main.cpp
    src/events.cpp
    src/layout.cpp
    src/obj.cpp
)

# Link SFML graphics library to the executable
target_link_libraries(PendulumExec PRIVATE sfml-graphics)
target_compile_features(PendulumExec PRIVATE cxx_std_11)

# Custom command to copy OpenAL DLL if on Windows
if(WIN32)
    add_custom_command(
        TARGET PendulumExec
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll
        $<TARGET_FILE_DIR:PendulumExec>
        VERBATIM
    )
endif()
