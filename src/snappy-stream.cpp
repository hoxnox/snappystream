#include <stdexcept>
#include <cstring>
#include <cstdio>
#include <stdint.h>

//#include "snappy-internal.h"
#include "snappy-stream.h"
#include "snappy.h"

namespace snappy {

const char magic[] = {'S','N','A','P'};

osnapstreambuf::osnapstreambuf(std::streambuf *dest, unsigned chunksize)
    : dest_(dest)
    , in_buffer_(new char[chunksize])
    , chunksize_(chunksize)
{
    this->init();
}

osnapstreambuf::~osnapstreambuf()
{
    this->sync();
    delete[] in_buffer_;
}

void osnapstreambuf::init()
{
    if (dest_->sputn(reinterpret_cast<const char*>(magic), 4)  != 4)
        throw std::runtime_error("can not write magic to osnapstreambuf");
    this->setp(in_buffer_, in_buffer_ + chunksize_ - 1);
}

void osnapstreambuf::terminate()
{
    sync();
    uint32_t terminator = 0;
    dest_->sputn(reinterpret_cast<const char*>(&terminator), 4);
    dest_->pubsync();
    setp(0,0);
}

osnapstreambuf::int_type osnapstreambuf::overflow(osnapstreambuf::int_type c)
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

int osnapstreambuf::sync()
{
    if (!pptr())
        return -1;

    std::streamsize uncompressed_len = pptr()-pbase();
    if (!uncompressed_len)
        return 0;

    uint32_t compressed_len = Compress(in_buffer_,
                                               uncompressed_len,
                                               &out_buffer_);
    uint32_t len_le = LittleEndian::FromHost32(compressed_len);
    if (dest_->sputn(reinterpret_cast<const char*>(&len_le), 4)  != 4)
        return -1;
    if (dest_->sputn(&out_buffer_[0], compressed_len) != compressed_len)
        return -1;
    pbump(-uncompressed_len);
    return uncompressed_len;
}

isnapstreambuf::isnapstreambuf(std::streambuf *src)
    : src_(src)
{
    char source_magic[4];
    std::streamsize nread = src_->sgetn(source_magic, 4);
    if (memcmp(magic, source_magic, 4))
        throw std::runtime_error("isnapstreambuf - bad magic number");
    this->setg(0,0,0);
}

isnapstreambuf::int_type isnapstreambuf::underflow()
{
    uint32_t len_le = 0;
    if (src_->sgetn(reinterpret_cast<char*>(&len_le), 4) != 4)
        return EOF;

    uint32_t len = LittleEndian::ToHost32(len_le);
    if (!len)
        return EOF;

    if (len > in_buffer_.size())
        in_buffer_.resize(len);

    if (src_->sgetn(reinterpret_cast<char*>(&in_buffer_[0]), len) != len)
        return EOF;

    size_t uncompressed_len = 0;
    if (!GetUncompressedLength(&in_buffer_[0], len, &uncompressed_len)
        || !uncompressed_len)
        return EOF;

    out_buffer_.resize(uncompressed_len);
    if (!RawUncompress(&in_buffer_[0], len, &out_buffer_[0]))
        return EOF;

    this->setg(&out_buffer_[0], &out_buffer_[0],
               &out_buffer_[0] + out_buffer_.size());
    return traits_type::to_int_type(*(this->gptr()));
}

isnapverifystreambuf::isnapverifystreambuf(std::streambuf *src)
    : isnapstreambuf(src)
    , nchunks_(0)
    , original_size_(0)
    , compressed_size_(4)
{
    out_buffer_.resize(1);
    out_buffer_[0] = 0;
}

isnapverifystreambuf::int_type isnapverifystreambuf::underflow()
{
    uint32_t len_le = 0;
    std::streamsize nr = src_->sgetn(reinterpret_cast<char*>(&len_le), 4);
    if (!nr)
        return EOF;
    if (nr != 4)
        throw std::runtime_error("malformed snapstream");

    uint32_t len = LittleEndian::ToHost32(len_le);
    compressed_size_ += 4;
    if (!len)
        return EOF;

    if (len > in_buffer_.size())
        in_buffer_.resize(len);

    if (src_->sgetn(reinterpret_cast<char*>(&in_buffer_[0]), len) != len)
        throw std::runtime_error("unexpected end of chunk");

    if (!IsValidCompressedBuffer(&in_buffer_[0], len))
        throw std::runtime_error("invalid compressed block");

    size_t uncompressed_len = 0;
    if (!GetUncompressedLength(&in_buffer_[0], len, &uncompressed_len)
        || !uncompressed_len)
        throw std::runtime_error("error parsing uncompressed length");

    ++nchunks_;
    compressed_size_ += len;
    original_size_ += uncompressed_len;

    this->setg(&out_buffer_[0], &out_buffer_[0],
               &out_buffer_[0] + out_buffer_.size());
    return traits_type::to_int_type(*(this->gptr()));
}

osnapstream::osnapstream(std::ostream *out, unsigned chunksize)
    : osbuf_(out->rdbuf(), chunksize)
    , std::ostream(&osbuf_)
{
}

void osnapstream::terminate()
{
    osbuf_.terminate();
}

isnapstream::isnapstream(std::istream *in)
    : isbuf_(in->rdbuf())
    , std::istream(&isbuf_)
{
}

} // namespace snappy
