////////////////////////////////////////////////////////////////////////////
// index.h
// Author: Merder Kim <hoxnox@gmail.com>
// Licanse: BSD
////////////////////////////////////////////////////////////////////////////

/**


<table border=0 width=100%>
<tr><td>
@author Merder Kim <hoxnox@gmail.com>
@date 20130112
</td><td width=150px>
</td></tr></table>

<hr/>

@mainpage SnappyStream

@tableofcontents

@section intro Introduction

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

@section quickstart Quick start

1. install snappy and cmake using emerge, apt, or whatever
2. execute 

   @cmd
   cd /tmp
   git clone git://github.com/hoxnox/snappy-stream.git
   cd snappy-stream
   mkdir build
   cd build
   cmake ../
   make
   g++ -I../include -L./ ../doc/examples/example_main.cpp -lsnappystream -lsnappy -oexample
   ./example
   @endcmd

3. Generate doc for futher reading (you will need doxygen)

   @cmd
   make doc
   @endcmd

@section exmaple Usage example

Building and linking (assume snappy-stream - directory with the project, snappy-stream/build - build dir:

@cmd
g++ -I"snappy-stream/include" -L"snappy-stream/build" example.cpp -lsnappystream
@endcmd

@include example_main.cpp


*/
