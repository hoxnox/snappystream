/**@author Merder Kim <hoxnox@gmail.com>
 * @date 20130117 22:25:30 */

#include <stdexcept>
#include <cstring>
#include <cstdio>
#include <stdint.h>
#include <string>

#include <osnappystream.hpp>
#include "snappy.h"
#include "crc32c.hpp"
#include "endians.hpp"


namespace snappy {

/**@class oSnappyStreambuf
 * @brief Snappy compressed output streambuf.
 *
 * Usage:
 * @example example.cpp*/

/**@brief Construct compressed streambuf, based on other streambuf*/
oSnappyStreambuf::oSnappyStreambuf(std::streambuf* dest, size_t chunksize)
	: dest_(dest)
	, write_cksums_(true)
	, in_buffer_(new char[chunksize])
	, chunksize_(chunksize)
{
	this->init();
}

/**@brief Sync and delete buffer on destruction*/
oSnappyStreambuf::~oSnappyStreambuf()
{
	this->sync();
	delete[] in_buffer_;
}

/**@brief Set boundaries of the controlled output sequence and write magic to dest_*/
void oSnappyStreambuf::init()
{
	if (dest_->sputn(reinterpret_cast<const char*>(Config::magic), 7)  != 7)
		throw std::runtime_error("can not write magic to oSnappyStreambuf");
	this->setp(in_buffer_, in_buffer_ + chunksize_ - 1);
}

/**@override std::streambuf::overflow(int)*/
oSnappyStreambuf::int_type oSnappyStreambuf::overflow(oSnappyStreambuf::int_type c)
{
	if (!pptr())
		return EOF;
	if (c != EOF) {
		*pptr() = c;
		pbump(1);
	}
	if (sync() == -1) {
		return EOF;
	}
	return c;
}

/**@override std::streambuf::sync()
 * @brief Flush data to dest_*/
int oSnappyStreambuf::sync()
{
	if (!pptr())
		return -1;

	std::streamsize uncompressed_len = pptr()-pbase();
	if (!uncompressed_len)
		return 0;

	uint32_t crc32c = write_cksums_ ? crc32c_masked(in_buffer_, uncompressed_len) : 0;


	char* compressed = new char[MaxCompressedLength(uncompressed_len)];
	size_t compressed_len_sz;
	RawCompress(in_buffer_, uncompressed_len, compressed, &compressed_len_sz);

	// use uncompressed input if less than 12.5% compression
	if (compressed_len_sz >= (uncompressed_len - (uncompressed_len / 8))) {
		delete [] compressed;
		return writeBlock(in_buffer_, uncompressed_len, uncompressed_len, false, crc32c);
	}
    std::streamsize compressed_len = static_cast<std::streamsize>(compressed_len_sz);
	int rs = writeBlock(compressed, uncompressed_len, compressed_len, true, crc32c);
	delete [] compressed;
	return rs;
}

int oSnappyStreambuf::writeBlock(const char * data, std::streamsize& uncompressed_len, std::streamsize& length, bool compressed, uint32_t cksum)
{
	BigEndian<uint16_t> len((uint16_t)length);
	BigEndian<uint32_t> cksum_be(cksum);
	if (dest_->sputc(compressed ? 1 : 0) == EOF)
		return -1;
	if (dest_->sputn(reinterpret_cast<const char*>(&len), 2) != 2)
		return -1;
	if (dest_->sputn(reinterpret_cast<const char*>(&cksum_be), 4) != 4)
		return -1;
	if (dest_->sputn(&data[0], length) != length)
		return -1;
	pbump(-uncompressed_len);
	return uncompressed_len;
}

/**@brief You can create compressed stream over every stream based on std::streambuf
 * @param chunksize The size of chunks*/
oSnappyStream::oSnappyStream(std::streambuf& outbuf, unsigned chunksize)
	: osbuf_(&outbuf, chunksize)
	, std::ostream(&osbuf_)
{
}

/**@brief You can create compressed stream over every stream based on std::ostream
 * @param chunksize The size of chunks*/
oSnappyStream::oSnappyStream(std::ostream& out, unsigned chunksize)
	: osbuf_(out.rdbuf(), chunksize)
	, std::ostream(&osbuf_)
{
}

} // namespace

