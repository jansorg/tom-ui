cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_MODULE_PATH cmake)

# Include Qt basic functions
include(cmake/QtCommon.cmake)

file(READ version.txt VERSION)

# Basic information about project
project(tom-ui
        LANGUAGES CXX
        VERSION ${VERSION}
        DESCRIPTION "Tom Time Tracker"
        HOMEPAGE_URL "https://github.com/jansorg/tom-ui")

# Set PROJECT_VERSION_PATCH and PROJECT_VERSION_TWEAK to 0 if not present, needed by add_project_meta
fix_project_version()

# Set additional project information
set(COMPANY "Joachim Ansorg")
set(COPYRIGHT "Copyright (c) 2019 Joachim Ansorg. All rights reserved.")
set(IDENTIFIER "eu.ja-dev.tomui")

set(CPACK_PACKAGE_CONTACT "mail@ja-dev.eu")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

add_project_meta(META_FILES_TO_INCLUDE)

set(RESOURCE_FILES resources/resources.qrc)

configure_file (
        "source/version.h.in"
        "${PROJECT_BINARY_DIR}/version.h"
)

file(GLOB_RECURSE CODE_FILES source/*.cpp source/*.h)

add_compile_options(-std=c++14 -Wall -Wextra)
if(CMAKE_BUILD_TYPE EQUAL "release")
    add_compile_options(-O3)
endif()

add_executable(${PROJECT_NAME} ${OS_BUNDLE} # Expands to WIN32 or MACOS_BUNDLE depending on OS
    ${CODE_FILES} ${META_FILES_TO_INCLUDE} ${RESOURCE_FILES}
)

if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/cmake/Info.plist.in
            MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_DESCRIPTION})
endif()

#if(APPLE)
#    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "tom-ui")
#endif()

target_include_directories(${PROJECT_NAME} PRIVATE source)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR})

# setup libraries
find_package(Qt5 COMPONENTS Core Widgets Svg WebEngineWidgets Test REQUIRED)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Svg Qt5::WebEngineWidgets Qt5::Test )

if (UNIX)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    install(FILES $ENV{HOME}/go/bin/tom DESTINATION bin)
elseif(APPLE)
    install(TARGETS ${PROJECT_NAME} DESTINATION .)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(${CMAKE_INSTALL_PREFIX}/tom-ui.app \"\" \"\")
    " COMPONENT Runtime)
elseif(WIN32)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()

INCLUDE(CPack)
