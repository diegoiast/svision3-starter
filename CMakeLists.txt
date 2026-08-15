cmake_minimum_required(VERSION 3.20)
project(svision3-starter LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Builds svision3 from source instead of consuming it as a prebuilt conan
# package. Self-contained: fetched into this project's own build tree, no
# dependency on a sibling checkout existing on disk.
# TODO: pin GIT_TAG to a real tag/commit instead of a moving branch.
FetchContent_Declare(svision3
    GIT_REPOSITORY https://github.com/diegoiast/svision3.git
    GIT_TAG        main
)

# We only need the library -- skip svision3's own demos/tests/task_manager.
set(SVISION3_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(SVISION3_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(svision3)

add_executable(SVision3Demo src/main.cpp)
target_link_libraries(SVision3Demo PRIVATE svision3::svision3)

set_target_properties(SVision3Demo PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME          "SVision3Demo"
    MACOSX_BUNDLE_BUNDLE_VERSION       "0.0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.0.1"
    # Without this, MSVC links with /SUBSYSTEM:CONSOLE by default, which
    # pops up a console window alongside the actual GUI window.
    WIN32_EXECUTABLE TRUE
)
if(WIN32)
    # WIN32_EXECUTABLE above switches to /SUBSYSTEM:WINDOWS, which expects a
    # WinMain entry point -- main.cpp uses a plain int main(), so the linker
    # needs to be told explicitly to still start from the C runtime's normal
    # entry point instead.
    set_target_properties(SVision3Demo PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

find_package(Catch2 REQUIRED)
include(CTest)
include(Catch)

add_executable(unit_tests tests/test_example.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(unit_tests)

install(TARGETS SVision3Demo
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
)

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    # Desktop menu entry + icon, matching what the AppImage packaging job
    # already generates inline -- deb/rpm need these too to show up in an
    # application launcher; cpack doesn't add them on its own. Not gated to
    # any specific CPack generator, so both DEB and RPM package this the
    # same way -- they share the same install() manifest, just archived
    # differently.
    install(FILES packaging/SVision3Demo.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    # One size per hicolor bucket -- desktop environments pick whichever
    # fits the context (taskbar, launcher grid, file manager, etc.) instead
    # of scaling a single size and losing sharpness.
    foreach(icon_size 32 64 128 256)
        install(FILES packaging/svision3demo-${icon_size}.png
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${icon_size}x${icon_size}/apps
            RENAME svision3demo.png)
    endforeach()
endif()

set(CPACK_PACKAGE_NAME    "SVision3Demo")
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_PACKAGE_VENDOR  "Your Company")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SVision3 starter demo application")

# Shown on its own wizard page (WIX) / in the installer (other generators).
# CPack's WIX generator wants a recognized extension (.txt or .rtf) to know
# how to convert it -- the bare LICENSE file doesn't have one, hence the copy.
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/packaging/LICENSE.txt")

# Generator-agnostic (WIX and NSIS both understand it): creates a Start Menu
# shortcut for the given executable, labelled with the second string.
set(CPACK_PACKAGE_EXECUTABLES "SVision3Demo" "SVision3Demo")

# Strip debug symbols from installed binaries when packaging (DEB/RPM).
set(CPACK_STRIP_FILES TRUE)

if(WIN32)
    set(CPACK_GENERATOR "WIX")
    # Regenerate this GUID if you fork or rename the product.
    set(CPACK_WIX_UPGRADE_GUID "E8A52B7C-3F4D-4A1E-9B6C-D2F8E3A1C057")
    # Shown in Add/Remove Programs and used for the Start Menu shortcut
    # created by CPACK_PACKAGE_EXECUTABLES above. WIX specifically wants a
    # .ico file -- CPACK_PACKAGE_ICON (used by NSIS) doesn't apply here.
    set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/packaging/svision3demo.ico")

    # Add/Remove Programs metadata.
    set(CPACK_WIX_PROPERTY_ARPCOMMENTS     "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
    set(CPACK_WIX_PROPERTY_ARPHELPLINK     "https://github.com/diegoiast/svision3-starter")
    set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://github.com/diegoiast/svision3-starter")

    # Banner shown at the top of most wizard pages (493x58) and the
    # full-page background used on the welcome/finish pages (493x312).
    set(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/packaging/wix-banner.bmp")
    set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/packaging/wix-dialog.bmp")

    # UNVERIFIED: no Windows/WiX toolchain available to test this against
    # (this project builds WIX packages only in CI). Overrides the
    # welcome/finish page wording via a localization file passed straight to
    # light.exe -- see packaging/wix-en-us.wxl for why a simple CPACK_WIX_*
    # variable can't do this. If the string Ids in that file don't match
    # what the installed WiX version expects, this silently has no effect
    # (the stock wording still shows) rather than breaking the build --
    # check the actual installer output in Windows CI and adjust the Ids in
    # wix-en-us.wxl if needed.
    set(CPACK_WIX_LIGHT_EXTRA_FLAGS "-loc" "${CMAKE_CURRENT_SOURCE_DIR}/packaging/wix-en-us.wxl")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "SVision3Demo")
elseif(UNIX)
    set(CPACK_GENERATOR "DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Company <info@yourcompany.com>")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)
