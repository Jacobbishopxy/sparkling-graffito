# @file:	CMakeLists.txt
# @author:	Jacob Xie
# @date:	2025/07/07 16:40:25 Monday
# @brief:

cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

###################################################################################################
# env file

# Function to read .env file
function(read_env_file env_file)
    file(STRINGS ${env_file} env_lines)
    foreach(line IN LISTS env_lines)
        if(NOT line MATCHES "^#" AND line MATCHES "=")
            string(REGEX REPLACE "=.*" "" key ${line})
            string(REGEX REPLACE "^[^=]*=" "" value ${line})
            string(REGEX REPLACE "^[\"'](.*)[\"']$" "\\1" value ${value})
            set(${key} ${value} PARENT_SCOPE)
        endif()
    endforeach()
endfunction()

if(EXISTS "${CMAKE_SOURCE_DIR}/.env")
    read_env_file("${CMAKE_SOURCE_DIR}/.env")
endif()

###################################################################################################

set(PROJECT_VERSION ${PROJ_VER})
project(SparklingGraffito VERSION ${PROJECT_VERSION} LANGUAGES CXX)
set(PROJ_PREFIX "/opt/${PROJECT_NAME}-${PROJECT_VERSION}")

include_directories(${CMAKE_SOURCE_DIR}/include)

###################################################################################################

# Link ZeroMQ statically
set(LIBZMQ_VERSION 4.3.5)
set(LIBZMQ_PATH "/opt/libzmq-${LIBZMQ_VERSION}/lib")
find_library(ZMQ_STATIC_LIBRARY
    NAMES libzmq.a
    PATHS ${LIBZMQ_PATH}
    NO_DEFAULT_PATH
    REQUIRED
)

add_library(libzmq STATIC IMPORTED)
set_target_properties(libzmq PROPERTIES IMPORTED_LOCATION "${ZMQ_STATIC_LIBRARY}")

# cppzmq is header-only
set(ZMQ_VERSION 4.11.0)
set(ZMQ_PATH "/opt/cppzmq-${ZMQ_VERSION}/include")
add_library(cppzmq INTERFACE)
target_include_directories(cppzmq INTERFACE ${ZMQ_PATH})
install(TARGETS cppzmq EXPORT ${PROJECT_NAME}Targets)

###################################################################################################

add_library(${PROJECT_NAME} STATIC
    src/Graffito.cpp
)

find_library(BSD_LIBRARY NAMES bsd)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}>
    $<INSTALL_INTERFACE:${PROJ_PREFIX}/include/${PROJECT_NAME}>
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${ZMQ_STATIC_LIBRARY} cppzmq)

# Hide all zmq symbols from public headers
target_compile_definitions(SparklingGraffito PRIVATE ZMQ_STATIC)


###################################################################################################

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${PROJ_PREFIX}/lib
    LIBRARY DESTINATION ${PROJ_PREFIX}/lib
    INCLUDES DESTINATION ${PROJ_PREFIX}/include
)

install(DIRECTORY include/${PROJECT_NAME}/
    DESTINATION ${PROJ_PREFIX}/include/${PROJECT_NAME}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${PROJ_PREFIX}/lib/cmake/${PROJECT_NAME}
)

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${PROJ_PREFIX}/lib/cmake/${PROJECT_NAME}
)

###################################################################################################

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${PROJ_PREFIX}/lib/cmake
)
