cmake_minimum_required(VERSION 3.16.0)

# Include Qt basic functions
set(CMAKE_MODULE_PATH cmake)
include(cmake/QtCommon.cmake)

file(READ version.txt VERSION)
string(STRIP "${VERSION}" VERSION)

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
set(COPYRIGHT "Copyright (c) 2019-2025 Joachim Ansorg. All rights reserved.")
set(IDENTIFIER "dev.tomtime.tomui")
set(CPACK_PACKAGE_CONTACT "mail@ja-dev.eu")
set(CPACK_PACKAGE_NAME "tom-ui")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "tom (>= ${VERSION})")
set(RESOURCE_FILES resources/resources.qrc source/settingsDialog/SettingsDialog.cpp source/settingsDialog/SettingsDialog.h source/settingsDialog/ApplicationSettingsPage.cpp source/settingsDialog/ApplicationSettingsPage.h source/settingsDialog/ApplicationShortcutsPage.cpp source/settingsDialog/ApplicationShortcutsPage.h source/settingsDialog/GlobalShortcutsPage.cpp source/settingsDialog/GlobalShortcutsPage.h source/GlobalShortcuts.cpp source/GlobalShortcuts.h source/GlobalShortcut.cpp source/GlobalShortcut.h)

include(GNUInstallDirs)

option(ENABLE_REPORTS "Enable Webkit component to preview reports" ON)

add_project_meta(META_FILES_TO_INCLUDE)

find_package(Qt6 6.4.0
        COMPONENTS
        Core
        Widgets
        Svg
        LinguistTools
        Test
        REQUIRED)
find_package(X11)

configure_file(
        "source/version.h.in"
        "${PROJECT_BINARY_DIR}/version.h"
)

file(GLOB_RECURSE CODE_FILES source/*.cpp source/*.h qxt/qxtglobalshortcut.cpp)
add_definitions(-DQXT_STATIC)

set(TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations/tom_de.ts)
# Update the translation sources with "cmake --build <build dir> --target translations".
# This isn't part of the regular build: it modifies files in the source tree, and running lupdate
# as a dependency of the build ran it concurrently on the same file, which lost the translations.
add_custom_target(translations
        COMMAND Qt6::lupdate -recursive ${CMAKE_CURRENT_SOURCE_DIR}/source -ts ${TS_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Updating translation sources"
        VERBATIM)

# generate qm files
qt6_add_translation(QM_FILES ${TS_FILES})
configure_file(translations/translations.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

if (Q_OS_MAC OR APPLE)
    file(GLOB CODE_FILES ${CODE_FILES} qxt/qxtglobalshortcut_mac.cpp)

    FIND_LIBRARY(CARBON_LIBRARY Carbon)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    LIST(APPEND TOM_LIBS ${CARBON_LIBRARY} ${COCOA_LIBRARY})
elseif (X11_FOUND)
    file(GLOB CODE_FILES ${CODE_FILES} qxt/qxtglobalshortcut_x11.cpp)
    list(APPEND TOM_LIBS ${X11_LIBRARIES})
elseif (Q_WS_WIN OR WIN32)
    file(GLOB CODE_FILES ${CODE_FILES} qxt/qxtglobalshortcut_win.cpp)
else ()
    message(WARNING "Building on unsupported platfom!")
    set(USE_QXT FALSE)
endif ()

# Global shortcuts on Wayland via the XDG desktop portal
if (UNIX AND NOT APPLE)
    find_package(Qt6 COMPONENTS DBus REQUIRED)
    list(APPEND CODE_FILES portal/XdgPortalGlobalShortcuts.cpp portal/XdgPortalGlobalShortcuts.h)
    list(APPEND TOM_LIBS Qt6::DBus)
    add_compile_definitions(TOM_XDG_PORTAL)
endif ()

if (MSVC)
    add_compile_options(/W3 /utf-8)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
    if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g3 -O0)
    endif ()
endif ()

add_executable(${PROJECT_NAME} ${OS_BUNDLE} # Expands to WIN32 or MACOS_BUNDLE depending on OS
        ${CODE_FILES} ${META_FILES_TO_INCLUDE} ${RESOURCE_FILES} ${QM_FILES} ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/cmake/Info.plist.in
            MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_DESCRIPTION})
endif ()

enable_testing()
#include_directories(test)

target_include_directories(${PROJECT_NAME} PRIVATE source)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR})
# add Qt6::Test to use model tester
target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Svg ${TOM_LIBS})

if (ENABLE_REPORTS)
    find_package(Qt6 OPTIONAL_COMPONENTS WebEngineWidgets)
    target_link_libraries(${PROJECT_NAME} Qt6::WebEngineWidgets)
    add_compile_definitions(TOM_REPORTS=true)
endif ()

if (UNIX)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    install(FILES resources/tom-ui.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    # application icon, named after the application ID like the .desktop file
    foreach (ICON_SIZE 16 24 32 48 64 128 256 512)
        install(FILES res/linux/${ICON_SIZE}x${ICON_SIZE}/tom-ui.png
                DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${ICON_SIZE}x${ICON_SIZE}/apps)
    endforeach ()
    install(FILES res/linux/scalable/tom-ui.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
    install(FILES res/linux/symbolic/tom-ui-symbolic.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/symbolic/apps)
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

if (WIN32)
    # Prepare the InnoSetup configuration script
    configure_file(deployment/setup.iss.in "${CMAKE_CURRENT_BINARY_DIR}/setup.iss")

    # The MSVC 2015 runtime requires CMake 3.6+
    if ((MSVC_VERSION VERSION_EQUAL 1900 OR MSVC_VERSION VERSION_GREATER 1900)
            AND CMAKE_VERSION VERSION_LESS "3.6")
        message(WARNING "MSVC 2015+ runtime requires CMake 3.6+")
    endif ()

    # Ensure the C/C++ runtime is installed
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)
endif ()

INCLUDE(CPack)
