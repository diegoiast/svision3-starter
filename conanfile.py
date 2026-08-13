from conan import ConanFile
from conan.tools.cmake import cmake_layout


class TwfmRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        # svision3 itself is built from source via FetchContent (see
        # CMakeLists.txt), not consumed as a conan package -- these are its
        # direct dependencies, mirrored here so conan's CMakeDeps still
        # generates find_package() configs for them. Keep in sync with
        # svision3/conanfile.py.
        self.requires("libcurl/8.21.0")
        self.requires("litehtml/0.8")
        self.requires("lunasvg/3.5.0")
        self.requires("md4c/0.5.2")
        self.requires("nlohmann_json/3.11.3")
        self.requires("spdlog/1.14.1")
        self.requires("tomlplusplus/3.4.0")

        if self.settings.os == "Linux":
            self.requires("stb/cci.20240213")

    def build_requirements(self):
        self.test_requires("catch2/[>=3.0 <4.0]")

    def layout(self):
        cmake_layout(self, build_folder="build")
