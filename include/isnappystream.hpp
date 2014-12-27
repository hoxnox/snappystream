/*@author Merder Kim <hoxnox@gmail.com>
 *@date 20130117 22:25:30 */

#include <iostream>
#include <vector>
#include <snappystreamcfg.hpp>

#ifndef __ISNAPPYSTREAM_HPP__
#define __ISNAPPYSTREAM_HPP__

namespace snappy {

class iSnappyStreambuf: public std::streambuf
{
	public:
		explicit iSnappyStreambuf(std::streambuf *src);

	protected:
		virtual int_type underflow();

	protected:
		std::streambuf* src_;
		std::vector<char_type> in_buffer_;
		std::vector<char_type> out_buffer_;
};

class iSnappyStream: public std::istream {
public:
	explicit iSnappyStream(std::streambuf& inbuf);
	explicit iSnappyStream(std::istream& in);
private:
	iSnappyStreambuf isbuf_;
};

} // namespace snappy

#endif // __ISNAPPYSTREAM_HPP__

