# @file:	CMakeLists.txt
# @author:	Jacob Xie
# @date:	2025/07/07 16:40:25 Monday
# @brief:

cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

###################################################################################################
# env file

# Function to read .env file
function(read_env_file env_file)
    file(STRINGS ${env_file} env_lines)
    foreach(line IN LISTS env_lines)
        # Skip comments and empty lines
        if(NOT line MATCHES "^#" AND line MATCHES "=")
            # Split into key and value
            string(REGEX REPLACE "=.*" "" key ${line})
            string(REGEX REPLACE "^[^=]*=" "" value ${line})

            # Remove surrounding quotes if present
            string(REGEX REPLACE "^[\"'](.*)[\"']$" "\\1" value ${value})

            # Set as CMake variable
            set(${key} ${value} PARENT_SCOPE)
        endif()
    endforeach()
endfunction()

# Usage
if(EXISTS "${CMAKE_SOURCE_DIR}/.env")
    read_env_file("${CMAKE_SOURCE_DIR}/.env")
endif()

###################################################################################################

set(PROJECT_VERSION ${PROJ_VER})

project(SparklingGraffito VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(PROJ_PREFIX "/opt/SparklingGraffito-${PROJECT_VERSION}")

add_library(SparklingGraffito STATIC
    src/Graffito.cpp
)

target_include_directories(SparklingGraffito
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/SparklingGraffito>
    $<INSTALL_INTERFACE:${PROJ_PREFIX}/include/SparklingGraffito>
)

###################################################################################################

install(TARGETS SparklingGraffito
    EXPORT SparklingGraffitoTargets
    ARCHIVE DESTINATION ${PROJ_PREFIX}/lib
    LIBRARY DESTINATION ${PROJ_PREFIX}/lib
    INCLUDES DESTINATION ${PROJ_PREFIX}/include
)

install(DIRECTORY include/SparklingGraffito/
    DESTINATION ${PROJ_PREFIX}/include/SparklingGraffito
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SparklingGraffitoConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SparklingGraffitoConfigVersion.cmake
    DESTINATION ${PROJ_PREFIX}/lib/cmake/SparklingGraffito
)

install(EXPORT SparklingGraffitoTargets
    FILE SparklingGraffitoTargets.cmake
    NAMESPACE SparklingGraffito::
    DESTINATION ${PROJ_PREFIX}/lib/cmake/SparklingGraffito
)

###################################################################################################

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SparklingGraffitoConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SparklingGraffitoConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SparklingGraffitoConfig.cmake
    INSTALL_DESTINATION ${PROJ_PREFIX}/lib/cmake/SparklingGraffito
)
