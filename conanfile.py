from conans import ConanFile, CMake

class SnappyStreamConan(ConanFile):
    name = "snappystream"
    description = "Snappy, fast compressor/decompressor streambuffer. See https://github.com/hoxnox/snappystream"
    license = "https://github.com/hoxnox/snappystream/blob/master/COPYING"
    version = "0.2.7"
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
            self.requires.add("Boost/1.60.0@lasote/stable", private=False)
            self.options["Boost"].shared = False

    def build(self):
        cmake = CMake(self)
        boost_iostreams_definition = ""
        if self.options.boost_iostreams:
            boost_iostreams_definition = "-DWITH_BOOST_IOSTREAMS=1"
        self.run('cmake -DWITH_CONAN=1 -DCMAKE_INSTALL_PREFIX=%s %s %s %s' %
                (self.package_folder, cmake.command_line,
                    boost_iostreams_definition, self.source_folder))
        self.run("cmake --build . --target install %s" % cmake.build_config)

    def package_info(self):
        self.cpp_info.libs = ["snappystream"]

