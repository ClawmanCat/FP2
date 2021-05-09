cmake_minimum_required(VERSION 3.12)
project("FP2")


# For testing purposes, it is often useful to control what flags get passed to the compiler,
# rather than relying on the current CMake profile.
option(CLEAR_FLAGS "Clear compiler flags for the current profile." OFF)

if (${CLEAR_FLAGS})
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()


# This flag is used in ./cmake/create_target.cmake to generate test targets if it is enabled.
option(ENABLE_TESTING "Enable tests." OFF)


# Prevent running CMake concurrently.
file(LOCK ${CMAKE_SOURCE_DIR} DIRECTORY GUARD FILE)


# C++20 features are required.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Output to out directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}/bin)


# Allow including from the root directory.
include_directories(${CMAKE_SOURCE_DIR})

# Allow loading of scripts from cmake folder.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Store builds in out folder.
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/out")


# Load compiler settings profile.
string(TOLOWER ${CMAKE_CXX_COMPILER_ID} compiler)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/profile_${compiler}.cmake)
    include("profile_${compiler}")
    set_compiler_profile()
endif()


# Run Conan
include(run_conan)
run_conan()


# Add Targets
add_subdirectory(imperative)
add_subdirectory(metaprogram)
add_subdirectory(semifunctional)