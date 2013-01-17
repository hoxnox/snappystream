/*@author Merder Kim <hoxnox@gmail.com> 
 *@date 20130117 22:25:30 */

#include <stdexcept>
#include <cstring>
#include <cstdio>
#include <stdint.h>
#include <string>


#include "isnappystream.hpp"
#include "snappy.h"
#include "crc32c.h"
#include "endians.hpp"

namespace snappy {

iSnappyStreambuf::iSnappyStreambuf(std::streambuf *src)
	: src_(src)
{
	char source_magic[Config::magic_sz];
	std::streamsize nread = src_->sgetn(source_magic, Config::magic_sz);
	if (memcmp(Config::magic, source_magic, Config::magic_sz))
		throw std::runtime_error("iSnappyStreambuf - bad magic number");
	this->setg(0, 0, 0);
}

iSnappyStreambuf::int_type iSnappyStreambuf::underflow()
{
	char header[7];
	if (src_->sgetn(header, 7) != 7)
		return EOF;

	bool compressed = true;
	if(header[0] == 0)
		compressed = false;
	BigEndian<uint16_t> len(&header[1], 2);
	uint32_t cksum = *reinterpret_cast<uint32_t*>(&header[3]);

	if (!len)
		return EOF;

	// expect, that the size of compressed data is less then uncompressed
	if (len > Config::defaultChunkSize) 
		return EOF;
	if (len > in_buffer_.size())
		in_buffer_.resize(len);

	int rs;
	if (src_->sgetn(reinterpret_cast<char*>(&in_buffer_[0]), len) != len)
		return EOF;

	size_t uncompressed_len = len;
	if(compressed)
	{
		if (!GetUncompressedLength(&in_buffer_[0], len, &uncompressed_len)
			|| !uncompressed_len)
			return EOF;
	}
	out_buffer_.resize(uncompressed_len);
	if(compressed)
	{
		if (!RawUncompress(&in_buffer_[0], len, &out_buffer_[0]))
			return EOF;
	}
	else
	{
		memcpy(&out_buffer_[0], &in_buffer_[0], uncompressed_len);
	}

	this->setg(&out_buffer_[0], &out_buffer_[0],
			   &out_buffer_[0] + out_buffer_.size());
	return traits_type::to_int_type(*(this->gptr()));
}

iSnappyStream::iSnappyStream(std::istream& in)
	: isbuf_(in.rdbuf())
	, std::istream(&isbuf_)
{
}

} // namespace snappy

