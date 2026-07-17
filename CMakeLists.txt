cmake_minimum_required(VERSION 3.24) # LINK_GROUP:RESCAN, needed by scintilla-svision3 below
project(twfm LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TWFM_BUILD_DEMO "Build the demo executable" ON)
option(TWFM_BUILD_TESTS "Build the unit_tests executable" ON)

if(TWFM_BUILD_DEMO)
    find_package(svision3 REQUIRED)
    # Not a direct twfm dependency, but already in the graph transitively via
    # svision3 (spdlog/1.14.1 in its conanfile.py) -- Conan's CMakeDeps already
    # generates its find-module here, so this just uses what's already fetched
    # rather than adding a new requirement, for demo/main.cpp's spdlog::info()
    # calls.
    find_package(spdlog REQUIRED)

    # Phase A (see ../scintilla-svision3/../scintilla-svision3-design.md):
    # scintilla-svision3 isn't packaged via conan yet, so it's pulled in as a
    # sibling checkout, same as it does for scintilla-code/lexilla itself.
    # EXCLUDE_FROM_ALL keeps its own demo/test executables out of `cmake --build`
    # by default -- only the scintilla-svision3/scintilla-core/lexilla library
    # targets this project's `demo` actually links against get built.
    set(BUILD_QT OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../scintilla-svision3 scintilla-svision3-build EXCLUDE_FROM_ALL)

    add_executable(demo src/main.cpp src/file_open_helpers.cpp)
    target_link_libraries(demo PRIVATE
        svision3::svision3
        spdlog::spdlog
        $<LINK_GROUP:RESCAN,scintilla-svision3,scintilla-core,lexilla>
    )

    set_target_properties(demo PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME          "SVision3Demo"
        MACOSX_BUNDLE_BUNDLE_VERSION       "0.0.1"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "0.0.1"
    )

    install(TARGETS demo
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION bin
    )
endif()

if(TWFM_BUILD_TESTS)
    find_package(Catch2 REQUIRED)
    include(CTest)
    include(Catch)

    add_executable(unit_tests tests/test_example.cpp tests/file_open_helpers_test.cpp src/file_open_helpers.cpp)
    target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
    catch_discover_tests(unit_tests)
endif()

set(CPACK_PACKAGE_NAME    "SVision3Demo")
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_PACKAGE_VENDOR  "Your Company")

if(WIN32)
    set(CPACK_GENERATOR "WIX")
    # Regenerate this GUID if you fork or rename the product.
    set(CPACK_WIX_UPGRADE_GUID "E8A52B7C-3F4D-4A1E-9B6C-D2F8E3A1C057")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "SVision3Demo")
elseif(UNIX)
    set(CPACK_GENERATOR "DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Company <info@yourcompany.com>")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)
