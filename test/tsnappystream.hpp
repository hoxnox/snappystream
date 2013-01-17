/**@author Merder Kim <hoxnox@gmail.com>
 * @date $date$
 * @license BSD 
 *
 * SnappyStream tests*/

#include <fstream>
#include <osnappystream.hpp>
#include <isnappystream.hpp>

class TestSnappyStream : public ::testing::Test
{
	protected:
		void SetUp()
		{
			osnstrm_ = new snappy::oSnappyStream(sstream_);
			isnstrm_ = new snappy::iSnappyStream(sstream_);
			for(size_t i = 0; i < 64000; ++i)
				hello_str_ += "Hello, world! ";
		}
		void TearDown()
		{
		}
		snappy::iSnappyStream * isnstrm_;
		snappy::oSnappyStream * osnstrm_;
		std::stringstream sstream_;
		std::string hello_str_;
};

TEST_F(TestSnappyStream, main)
{
	*osnstrm_ << hello_str_ << std::endl;
	ASSERT_EQ(sstream_ .str().length(), 42708);
	std::string str;
	getline(*isnstrm_, str);
	//*isnstrm_ >> str;
	ASSERT_EQ(str, hello_str_);
}

