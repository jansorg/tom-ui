cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_MODULE_PATH cmake)

option(ENABLE_REPORTS "Enable Webkit component to preview reports" ON)

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
set(CPACK_PACKAGE_NAME "tom-ui")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "tom (>= ${VERSION})")
set(RESOURCE_FILES resources/resources.qrc)

add_project_meta(META_FILES_TO_INCLUDE)

find_package(Qt5 COMPONENTS Core Widgets Svg LinguistTools Test REQUIRED)
find_package(X11)

configure_file(
        "source/version.h.in"
        "${PROJECT_BINARY_DIR}/version.h"
)

file(GLOB_RECURSE CODE_FILES source/*.cpp source/*.h qxt/qxtglobalshortcut.cpp)
add_definitions(-DQXT_STATIC)

set(TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations/tom_de.ts)
file(GLOB_RECURSE TS_SOURCES "source/*.cpp" "source/*.h" "source/*.ui")
add_custom_target(translations)
foreach (_ts_file ${TS_FILES})
    # generate a sensible name for this translation file
    get_filename_component(_ts_name ${_ts_file} NAME_WE)
    # call lupdate ourselves
    add_custom_command(
            OUTPUT "${_ts_file}"
            DEPENDS ${TS_SOURCES}
            COMMAND ${Qt5_LUPDATE_EXECUTABLE} -recursive ${CMAKE_CURRENT_SOURCE_DIR} -ts ${_ts_file}
    )
    # create a target that depends on the generated files
    add_custom_target(translate_${_ts_name} DEPENDS ${_ts_file})
    # attach the custom target for this ts file to the parent target
    add_dependencies(translations translate_${_ts_name})
endforeach ()

# generate qm files
qt5_add_translation(QM_FILES ${TS_FILES})
configure_file(translations/translations.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

if (X11_FOUND)
    file(GLOB CODE_FILES ${CODE_FILES} qxt/qxtglobalshortcut_x11.cpp)
    list(APPEND TOM_LIBS ${X11_LIBRARIES})
elseif (Q_WS_WIN OR WIN32)
    file(GLOB CODE_FILES ${CODE_FILES} qxt/qxtglobalshortcut_win.cpp)
elseif (Q_OS_MAC OR APPLE)
    file(GLOB CODE_FILES ${CODE_FILES} qxt/qxtglobalshortcut_mac.cpp)

    FIND_LIBRARY(CARBON_LIBRARY Carbon)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    LIST(APPEND TOM_LIBS ${CARBON_LIBRARY} ${COCOA_LIBRARY})
else ()
    message(WARNING "Building on unsupported platfom!")
    set(USE_QXT FALSE)
endif ()

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

if (WIN32)
    add_compile_options()
else()
    add_compile_options(-std=c++14 -Wall -Wextra -pedantic)
endif()

if (CMAKE_BUILD_TYPE EQUAL "release")
    add_compile_options(-O3)
else()
    add_compile_options(-g)
endif ()

add_executable(${PROJECT_NAME} ${OS_BUNDLE} # Expands to WIN32 or MACOS_BUNDLE depending on OS
        ${CODE_FILES} ${META_FILES_TO_INCLUDE} ${RESOURCE_FILES} ${QM_FILES} ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc)

if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/cmake/Info.plist.in
            MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_DESCRIPTION})
endif ()

#enable_testing(true)
#include_directories(test)

target_include_directories(${PROJECT_NAME} PRIVATE source)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR})
# add Qt5::Test to use model tester
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Svg ${TOM_LIBS})

if (ENABLE_REPORTS)
    find_package(Qt5 OPTIONAL_COMPONENTS WebEngineWidgets)
    target_link_libraries(${PROJECT_NAME} Qt5::WebEngineWidgets)
    add_compile_definitions(TOM_REPORTS=true)
endif ()

if (UNIX)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
elseif (APPLE)
    install(TARGETS ${PROJECT_NAME} DESTINATION .)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(${CMAKE_INSTALL_PREFIX}/tom-ui.app \"\" \"\")
    " COMPONENT Runtime)
elseif (WIN32)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif ()

if(WIN32)
    # Prepare the InnoSetup configuration script
    configure_file(deployment/setup.iss.in "${CMAKE_CURRENT_BINARY_DIR}/setup.iss")

    # The MSVC 2015 runtime requires CMake 3.6+
    if((MSVC_VERSION VERSION_EQUAL 1900 OR MSVC_VERSION VERSION_GREATER 1900)
            AND CMAKE_VERSION VERSION_LESS "3.6")
        message(WARNING "MSVC 2015+ runtime requires CMake 3.6+")
    endif()

    # Ensure the C/C++ runtime is installed
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)
endif()

INCLUDE(CPack)
