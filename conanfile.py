from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake

class SnappyStreamConan(ConanFile):
    name = "snappystream"
    description = "Snappy, fast compressor/decompressor streambuffer. See https://github.com/hoxnox/snappystream"
    license = "https://github.com/hoxnox/snappystream/blob/master/COPYING"
    version = "1.0.0"
    requires = "snappy/1.1.9"
    settings = "os", "compiler", "build_type", "arch"
    options = {"boost_iostreams": [True, False]}
    default_options = {"boost_iostreams":False, "snappy/*:shared": False}
    exports_sources = ("include/*.hpp",
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
            self.requires.add("boost/[>=1.44.0]", private=False)
            self.options["boost"].shared = False

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["WITH_BOOST_IOSTREAMS"] = self.options.boost_iostreams
        tc.variables["WOTH_CONAN"] = True
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["snappystream"]

