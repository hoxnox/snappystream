from conans import ConanFile, CMake
import os

class SnappyStreamTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "snappystream/0.2.7@hoxnox/stable"
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        self.run('cmake "%s" %s' % (self.source_folder, cmake.command_line))
        self.run("cmake --build . %s" % cmake.build_config)

    def imports(self):
        self.copy("*.dll", "bin", "bin")
        self.copy("*.dylib", "bin", "lib")

    def test(self):
        os.chdir("bin")
        self.run(".%spkg_test" % os.sep)
