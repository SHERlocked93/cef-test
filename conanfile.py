import os
from conan import ConanFile
from conan.tools.files import copy


class MyProjectConan(ConanFile):
    name = "cef_131_mytest"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "MSBuildDeps", "MSBuildToolchain"

    def requirements(self):
        self.requires("nlohmann_json/3.11.3")
        self.requires("sqlite3/3.48.0", options={"shared": True})

    def layout(self):
        self.folders.generators = os.path.join("thirdparty", "conan")
        self.folders.build = os.path.join("thirdparty", "conan", "build")

    def deploy(self):
        print(f"  -->deploying env: arch={self.settings.arch}, build_type={self.settings.build_type}")

        source_dir = os.path.join(self.dependencies["sqlite3"].package_folder, "bin")
        print(f"  -->deploy source_dir: {source_dir}")

        # 动态生成目标路径（如 x64/Debug 或 x64/Release）
        arch = "x64" if self.settings.arch == "x86_64" else "x86"
        target_dir = os.path.join(os.getcwd(), arch, str(self.settings.build_type))
        print(f"  -->deploy target_dir: {target_dir}")

        # 复制 DLL 到项目输出目录
        dll_files = [f for f in os.listdir(source_dir) if f.endswith(".dll")]
        for dll_file in dll_files:
            print(f"    --> Copying {dll_file}")
            copy(self, dll_file, src=source_dir, dst=target_dir)
