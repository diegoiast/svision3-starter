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

    def build_requirements(self):
        self.test_requires("catch2/[>=3.0 <4.0]")

    def layout(self):
        cmake_layout(self, build_folder="build")
