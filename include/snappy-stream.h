#ifndef UTIL_SNAPPY_SNAPPYSTREAM_H__
#define UTIL_SNAPPY_SNAPPYSTREAM_H__

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <stdint.h>

namespace snappy {

class osnapstreambuf: public std::streambuf {
public:
    explicit osnapstreambuf(std::streambuf *dest,
                            unsigned chunksize = defaultChunkSize);
    virtual ~osnapstreambuf();
    virtual void terminate();

public:
    static const unsigned defaultChunkSize = 32*1024;

protected:
    virtual int_type overflow(int_type c = traits_type::eof());
    virtual int sync();
    virtual void init();

protected:
    std::streambuf* dest_;

private:
    char* in_buffer_;
    std::string out_buffer_;
    unsigned chunksize_;
};

class isnapstreambuf: public std::streambuf {
public:
    explicit isnapstreambuf(std::streambuf *src);

protected:
    virtual int_type underflow();

protected:
    std::streambuf* src_;
    std::vector<char_type> in_buffer_;
    std::vector<char_type> out_buffer_;
};

class isnapverifystreambuf: public isnapstreambuf {
public:
    explicit isnapverifystreambuf(std::streambuf *src);
    uint64_t nchunks() const {
        return nchunks_;
    }
    uint64_t original_size() const {
        return original_size_;
    }
    uint64_t compressed_size() const {
        return compressed_size_;
    }
protected:
    virtual int_type underflow();
private:
    uint64_t nchunks_;
    uint64_t original_size_;
    uint64_t compressed_size_;
};

class oSnappyStream: public std::ostream {
public:
    explicit oSnappyStream(std::ostream *out,
                         unsigned chunksize = defaultChunkSize);
    void terminate();
private:
    static const unsigned defaultChunkSize = oSnappyStream::defaultChunkSize;
    osnapstreambuf osbuf_;
};

class iSnappyStream: public std::istream {
public:
    explicit iSnappyStream(std::istream *in);
private:
    isnapstreambuf isbuf_;
};

} // namespace snappy

#endif
