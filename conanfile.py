from conan import ConanFile
from conan.tools.cmake import cmake_layout


class TwfmRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("svision3/0.0.1")

    def build_requirements(self):
        self.test_requires("catch2/[>=3.0 <4.0]")

    def layout(self):
        cmake_layout(self, build_folder="build")
