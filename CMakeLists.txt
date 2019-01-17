cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_MODULE_PATH cmake)

# Include Qt basic functions
include(cmake/QtCommon.cmake)

# Basic information about project
project(tom-ui VERSION 1.0.0)

# Set PROJECT_VERSION_PATCH and PROJECT_VERSION_TWEAK to 0 if not present, needed by add_project_meta
fix_project_version()

# Set additional project information
set(COMPANY "Joachim Ansorg")
set(COPYRIGHT "Copyright (c) 2019 Joachim Ansorg. All rights reserved.")
set(IDENTIFIER "eu.ja-dev.tom-ui")

add_project_meta(META_FILES_TO_INCLUDE)

set(RESOURCE_FILES resources/resources.qrc)

file(GLOB_RECURSE UI_FILES source/*.ui)
file(GLOB_RECURSE CODE_FILES source/*.cpp)

add_executable(${PROJECT_NAME} ${OS_BUNDLE} # Expands to WIN32 or MACOS_BUNDLE depending on OS
    ${CODE_FILES} ${META_FILES_TO_INCLUDE} ${RESOURCE_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE source)

# setup libraries
find_package(Qt5 COMPONENTS Core Widgets Svg REQUIRED)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Svg)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)