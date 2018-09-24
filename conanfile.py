from conans import ConanFile, CMake

class SnappyStreamConan(ConanFile):
    name = "snappystream"
    description = "Snappy, fast compressor/decompressor streambuffer. See https://github.com/hoxnox/snappystream"
    license = "https://github.com/hoxnox/snappystream/blob/master/COPYING"
    version = "0.2.8"
    requires = "snappy/1.1.4@hoxnox/stable"
    settings = "os", "compiler", "build_type", "arch"
    options = {"boost_iostreams": [True, False]}
    default_options = "boost_iostreams=False", "snappy:shared=False"
    generators = "cmake"
    exports = ("include/*.hpp",
               "src/*.hpp", "src/*.cpp",
               "misc/snap.cpp",
               "snappystream.cfg",
               "cmake/ext/nx_utils.cmake",
               "cmake/Modules/FindSnappy.cmake",
               "cmake/Modules/gtest.cmake",
               "cmake/Modules/Vendoring.cmake",
               "CMakeLists.txt",
               "README.markdown")
    url = "https://github.com/hoxnox/snappystream.git"

    def config(self):
        if self.options.boost_iostreams:
            self.requires.add("boost/[>=1.44.0]@conan/stable", private=False)
            self.options["boost"].shared = False

    def build(self):
        cmake = CMake(self)
        cmake.definitions["WITH_BOOST_IOSTREAMS"] = self.options.boost_iostreams
        cmake.definitions["WITH_CONAN"] = True
        cmake.configure()
        cmake.build()
        cmake.install()

    def package(self):
        pass

    def package_info(self):
        self.cpp_info.libs = ["snappystream"]
