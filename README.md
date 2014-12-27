# C++ snappy stream

## Introduction

There is [snappy compressor from google][snappy], but it doesn't have default stream realisation.
This project aim to release C++ stream for snappy with format, compatible with [java realisation by
Dain Sundstrom][snjava]. So you can use, for example, java realisation for compression and C++ for
decompression, or vice versa.

Format description (from javadoc of Dain Sundstorm project):

	   ---+---+---+---+---+---+---+---+---+...+---+---
	...   |CFL|SIZE   |CRC32C         |DATA       |   ...
	   ---+---+---+---+---+---+---+---+---+...+---+---

	CFL    - indicating if the block is compressed or not. A value of 0x00 means uncompressed, and
	         0x01 means compressed.
	SIZE   - size of block in network byte order. This value is never zero as empty blocks are
	         never written. The maximum allowed length is 32k.
	CRC32C - crc32c checksum of the user input data masked with the following function: 
	         ((crc >>> 15) | (crc << 17)) + 0xa282ead8
	
	An uncompressed block is simply copied from the input, thus guaranteeing that the output is
	never larger than the input (not including the header)

[snappy]:http://code.google.com/p/snappy/
[snjava]:http://github.com/dain/snappy#stream-format

## Quick start

install snappy and cmake using emerge, apt, or whatever

execute 

	cd /tmp
	git clone git://github.com/hoxnox/snappy-stream.git
	cd snappy-stream
	mkdir build
	cd build
	cmake ../
	make
	g++ -I../include -L./ ../doc/examples/example_main.cpp -lsnappystream -lsnappy -oexample
	./example

Generate doc for futher reading (you will need doxygen)

	make doc

## Usage example

Building and linking (assume snappy-stream - directory with the project, snappy-stream/build - build dir:

	g++ -I"snappy-stream/include" -L"snappy-stream/build" example.cpp -lsnappystream

example:

	#include <fstream>
	#include <snappystream.hpp>
	
	void write()
	{
		std::ofstream ofile("snappy-file");
		if(!ofile.is_open())
			return;
		snappy::oSnappyStream osnstrm(ofile);
		osnstrm << "Hello, world!" << std::endl;
	}
	
	void read()
	{
		std::ifstream ifile("snappy-file");
		if(!ifile.is_open())
			return;
		snappy::iSnappyStream isnstrm(ifile);
		std::cout << isnstrm.rdbuf();
	}
	
	int main(int argc, char * argv[])
	{
		write();
		read();
		return 0;
	}

## Boost iostreams filters

You can use snappystream with boost::iostreams. Project must be
configured with `WITH_BOOST_IOSTREAMS` key:

	cmake -DWITH_BOOST_IOSTREAMS=1 ./

In that case snappystream.hpp includes OutputSnappyStreamBoostFilter.hpp
and InputSnappyStreamBoostFilter.hpp.

Usage example:

		namespace io = boost::iostreams;

		std::ofstream file("/path/to/file", std::ios::binary | std::ios::out);
		io::filtering_ostream out;
		out.push(snappy::OutputSnappyStreamBoostFilter());
		out.push(file);
		out << ""

		std::ifstream file("/path/to/file", std::ios::binary | std::ios::in);
		io::filtering_istream in;
		in.push(snappy::InputSnappyStreamBoostFilter());
		in.push(file);
		io::copy(in, std::cout);

