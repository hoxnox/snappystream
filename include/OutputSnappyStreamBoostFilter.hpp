/**@author $username$ <$usermail$>
 * @date $date$ */

#ifndef __OUTPUTSNAPPYSTREAMBOOSTFILTER_HPP__
#define __OUTPUTSNAPPYSTREAMBOOSTFILTER_HPP__

#include <iostream>
#include <ctype.h>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/scoped_ptr.hpp>
#include <snappystream.hpp>

namespace snappy {

class OutputSnappyStreamBoostFilter
{
public:
	typedef char                                         char_type;
	typedef boost::iostreams::multichar_output_filter_tag category;
	OutputSnappyStreamBoostFilter()
		: snstrm_(NULL)
	{
	}
	OutputSnappyStreamBoostFilter(const OutputSnappyStreamBoostFilter& copy)
		: snstrm_(NULL)
	{
	}
	OutputSnappyStreamBoostFilter& operator=(const OutputSnappyStreamBoostFilter& copy)
	{
		snstrm_.reset(NULL);
		return *this;
	}

	template<typename Sink>
	std::streamsize write(Sink& snk, const char* s, std::streamsize n)
	{
		if(!snstrm_)
			snstrm_.reset(new snappy::oSnappyStream(snk));
		if (!snstrm_->good())
			return 0;
		snstrm_->write(s, n);
		snstrm_->flush();
		if(snstrm_->bad())
			return 0;
		return n;
	}

private:
	boost::scoped_ptr<snappy::oSnappyStream> snstrm_;
};

} // namespace

#endif // __OUTPUTSNAPPYSTREAMBOOSTFILTER_HPP__

