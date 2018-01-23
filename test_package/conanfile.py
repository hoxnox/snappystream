from conans import ConanFile, CMake, tools, RunEnvironment
import os

class SnappyStreamTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", "bin", "bin")
        self.copy("*.dylib", "bin", "lib")

    def test(self):
        with tools.environment_append(RunEnvironment(self).vars):
            with tools.chdir('bin'):
                self.run("pkg_test")
