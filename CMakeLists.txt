cmake_minimum_required(VERSION 3.20)
project(twfm LANGUAGES C CXX)

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

add_executable(demo src/main.cpp)
target_link_libraries(demo PRIVATE svision3::svision3)

set_target_properties(demo PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME          "SVision3Demo"
    MACOSX_BUNDLE_BUNDLE_VERSION       "0.0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.0.1"
)

find_package(Catch2 REQUIRED)
include(CTest)
include(Catch)

add_executable(unit_tests tests/test_example.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(unit_tests)

install(TARGETS demo
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
)

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
