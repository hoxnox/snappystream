/**@author $username$ <$usermail$>
 * @date $date$ */

#ifndef __INPUTSNAPPYSTREAMBOOSTFILTER_HPP__
#define __INPUTSNAPPYSTREAMBOOSTFILTER_HPP__

#include <iostream>
#include <ctype.h>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/scoped_ptr.hpp>
#include <snappystream.hpp>

namespace snappy {

class InputSnappyStreamBoostFilter
{
public:
	typedef char                                         char_type;
	typedef boost::iostreams::multichar_input_filter_tag category;
	InputSnappyStreamBoostFilter()
		: snstrm_(NULL)
	{
	}
	InputSnappyStreamBoostFilter(const InputSnappyStreamBoostFilter& copy)
		: snstrm_(NULL)
	{
	}
	InputSnappyStreamBoostFilter& operator=(const InputSnappyStreamBoostFilter& copy)
	{
		snstrm_.reset(NULL);
		return *this;
	}

	template<typename Source>
	std::streamsize read(Source& src, char* s, std::streamsize n)
	{
		if(!snstrm_)
			snstrm_.reset(new snappy::iSnappyStream(src));
		if (snstrm_->eof())
			return -1;
		if (!snstrm_->good())
			return 0;
		snstrm_->read(s, n);
		if(snstrm_)
			return snstrm_->gcount();
		if(snstrm_->eof())
			return -1;
		return 0;
	}

private:
	boost::scoped_ptr<snappy::iSnappyStream> snstrm_;
};

} // namespace

#endif // __INPUTSNAPPYSTREAMBOOSTFILTER_HPP__

