import os
from conan import ConanFile

class MyProjectConan(ConanFile):
    name = "cef_131_mytest"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "MSBuildDeps", "MSBuildToolchain"

    def requirements(self):
        self.requires("nlohmann_json/3.11.3")

    def layout(self):
        self.folders.generators = os.path.join("thirdparty", "conan")
        self.folders.build = os.path.join("thirdparty", "conan", "build")
