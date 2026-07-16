from conan import ConanFile
from conan.tools.cmake import cmake_layout


class TwfmRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        # Tracks the current local svision3 checkout's version string
        # (svision3/conanfile.py's `version =`), not a released 0.0.1 --
        # rebuild with `conan create ~/src/diego/svision3` after pulling
        # svision3 changes.
        self.requires("svision3/0.0.1-dev")
        # Already pulled in transitively via svision3, but only as a build
        # dependency of svision3 itself -- its Debug-config CMake target
        # data came back empty when relying on that alone (a Conan package-
        # variant mismatch, not a real header-only/missing-config gap).
        # Requiring it directly here gives twfm's own conan install a
        # properly resolved Debug variant for main.cpp's spdlog::info() calls.
        self.requires("spdlog/1.14.1")

    def build_requirements(self):
        self.test_requires("catch2/[>=3.0 <4.0]")

    def layout(self):
        cmake_layout(self, build_folder="build")
