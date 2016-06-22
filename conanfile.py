from conans import ConanFile, CMake

class SnappyStreamConan(ConanFile):
    name = "snappystream"
    version = "0.2.3"
    requires = "snappy/1.1.3@hoxnox/testing"
    settings = "os", "compiler", "build_type", "arch"
    options = {"boost_iostreams": [True, False]}
    default_options = "boost_iostreams=False", "snappy:shared=False"
    generators = "cmake"
    exports = ("include/*.hpp",
               "src/*.hpp", "src/*.cpp",
               "misc/snap.cpp",
               "snappystream.cfg",
               "cmake/ext/nx_utils.cmake",
               "CMakeLists.txt",
               "README.markdown")
    url = "https://github.com/hoxnox/snappystream.git"

    def build(self):
        cmake = CMake(self.settings)
        boost_iostreams_definition = ""
        if self.options.boost_iostreams:
            boost_iostreams_definition = "-DWITH_BOOST_IOSTREAMS=1"
        self.run('cmake %s -DWITH_CONAN=1 -DCMAKE_INSTALL_PREFIX=./distr %s %s' %
                (self.conanfile_directory, cmake.command_line, boost_iostreams_definition))
        self.run("cmake --build . %s" % cmake.build_config)
        self.run("make install")


    def package(self):
        self.copy("*.hpp", dst="include", src="distr/include")
        self.copy("*.a", dst="lib", src="distr/lib")

    def package_info(self):
        self.cpp_info.libs = ["snappystream"]

