/**@author $username$ <$usermail$>
 * @date $date$ */

#include <vector>
#include <string>
#include <stdint.h>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/filesystem.hpp>
#include <snappystream.hpp>


namespace io = boost::iostreams;

class TestBoostIOFilter : public ::testing::Test
{
protected:
	TestBoostIOFilter()
		: hello_world("Hello world world world world world")
		, tmpfile("/tmp/__meridian_test_boost_gzip.gz")
	{
	}
	bool check_file(const char* etalon, const size_t etalonsz)
	{
		std::ifstream file(tmpfile.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
		size_t filesz = file.tellg();
		if (filesz != etalonsz)
			return false;
		file.seekg(0, file.beg);
		std::istreambuf_iterator<char> reader(file.rdbuf()), end();
		if (std::equal(etalon, etalon + etalonsz, reader))
			return true;
		else
			return false;
	}
	void SetUp()
	{
		//ASSERT_FALSE(fs::exists(tmpfile));
	}
	void TearDown()
	{
		//fs::remove(tmpfile);
	}
	const std::string hello_world;
	const std::string tmpfile;
};

#ifdef WITH_ZLIB
TEST_F(TestBoostIOFilter, gzip)
{
	{
		io::filtering_ostream out;
		out.push(io::gzip_compressor());
		out.push(io::file_sink(tmpfile.c_str()));
		out << hello_world;
	}
	const char gzipped_helloworld[] = {
		0x1F, 0x8B, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
		0xF3, 0x48, 0xCD, 0xC9, 0xC9, 0x57, 0x28, 0xCF, 0x2F, 0xCA,
		0x49, 0xC1, 0x45, 0x02, 0x00, 0xC4, 0x2C, 0x22, 0x0E, 0x23,
		0x00, 0x00, 0x00
	};
	EXPECT_TRUE(check_file(gzipped_helloworld, sizeof(gzipped_helloworld)));
}
#endif // WITH_ZLIB

TEST_F(TestBoostIOFilter, snappy)
{
	{
		std::ofstream file(tmpfile.c_str(), std::ios::binary | std::ios::out);
		io::filtering_ostream out;
		out.push(snappy::OutputSnappyStreamBoostFilter());
		out.push(file);
		size_t middle = hello_world.length()/2;
		out << hello_world.substr(0, middle);
		for (size_t i = middle; i < hello_world.length(); ++i)
			out << hello_world[i];
	}
	const uint8_t snapped_helloworld[] = {
		0x73, 0x6E, 0x61, 0x70, 0x70, 0x79, 0x00, 0x01, 0x00, 0x10,
		0x16, 0xB8, 0xDD, 0x69, 0x23, 0x28, 0x48, 0x65, 0x6C, 0x6C,
		0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x5E, 0x06, 0x00
	};
	ASSERT_TRUE(check_file((char*)snapped_helloworld, sizeof(snapped_helloworld)));

	{
		namespace io = boost::iostreams;
		std::ifstream file(tmpfile.c_str(), std::ios::binary | std::ios::in);
		io::filtering_istream in;
		in.push(snappy::InputSnappyStreamBoostFilter());
		in.push(file);
		std::stringstream ss;
		io::copy(in, ss);
		std::string s = ss.str();
		EXPECT_EQ(hello_world, s);
	}
}

