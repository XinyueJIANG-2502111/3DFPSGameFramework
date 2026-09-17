cmake_minimum_required(VERSION 3.20)

project(3DFPSGameFramework LANGUAGES CXX)

# ------------------------------------------------------------
# C++ Standard Configuration
# ------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------
# DxLib External Library Configuration
# ------------------------------------------------------------
set(DXLIB_DIR "C:/DxLib_VC/プロジェクトに追加すべきファイル_VC用" CACHE PATH "Path to DxLib VC package directory")

if(NOT EXISTS "${DXLIB_DIR}")
    message(FATAL_ERROR "DxLib directory not found at: ${DXLIB_DIR}\nPlease specify with -DDXLIB_DIR=<path>")
else()
    message(STATUS "Found DxLib directory at: ${DXLIB_DIR}")
endif()

# ------------------------------------------------------------
# Compiler & Runtime Configuration (MSVC)
# ------------------------------------------------------------
if(MSVC)
    # DxLib requires statically-linked C++ runtime (/MT in Release, /MTd in Debug)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    # DxLib MultiByte character set definition
    add_compile_definitions(_MBCS)

    # Standard conforming behavior
    add_compile_options(/permissive- /W3)
endif()

# ------------------------------------------------------------
# Source Files
# ------------------------------------------------------------
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# Organize source files in Visual Studio Solution Explorer
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SRC_FILES})

# ------------------------------------------------------------
# Executable Target
# ------------------------------------------------------------
# WIN32 creates a Windows GUI application (subsystem:windows / WinMain)
add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${DXLIB_DIR}"
)

# DxLib automatically specifies required .lib dependencies via #pragma comment(lib, ...) in its headers.
# We supply the library search directory so the linker can locate them.
target_link_directories(${PROJECT_NAME} PRIVATE
    "${DXLIB_DIR}"
)

# Set the working directory for Visual Studio debugger to project root (where Assets are located)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
