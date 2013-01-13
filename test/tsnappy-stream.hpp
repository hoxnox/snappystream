/**@author Merder Kim <hoxnox@gmail.com>
 * @date $date$
 * @license BSD 
 *
 * SnappyStream tests*/

class TestSnappyStream : public ::testing::Test
{
	protected:
		void SetUp()
		{
			isnstrm = new iStringStream(sstream_, 64);
			osnstrm = new oStringStream(sstream_);
		}
		void TearDown()
		{
		}
		iSnappyStream * isnstrm_;
		oSnappyStream * osnstrm_;
		std::stringstream sstream_;
};

TEST_F(TestSnappyStream, main)
{
	osnstrm = new iSnappyStream("temp.file");
	ASSERT_TRUE(osnstrm->is_open());
	osnstrm << "Hello, world!" << std::endl;
	osnstrm->close();
	std::string str;
	isnstrm = new oSnappyStream("temp.file");
	ASSERT_TRUE(isnstrm->is_open());
	isnstrm >> str;
	ASSERT_EQ(str, "Hello, world!");
	isnstrm->close();
}

