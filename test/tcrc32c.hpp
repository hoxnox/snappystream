/**@author Merder Kim <hoxnox@gmail.com>
 * @date $date$
 *
 * CRC32C tests*/

#include <crc32c.h>

class TestCRC32C : public ::testing::Test
{
	protected:
		void SetUp()
		{
		}
		void TearDown()
		{
		}
};

TEST_F(TestCRC32C, crc)
{
	EXPECT_EQ(crc32c_masked("test", 4), 2281961496U);
	EXPECT_EQ(crc32c_masked("hello", 5), 421273531);
	EXPECT_EQ(crc32c_masked("Hello, world!", 13), 2957933594U);
	EXPECT_EQ(crc32c_masked("Hello, world! Hello, world! Hello, world!", 13*3 + 2), 1197119801);
	const char str[] = "1234567890-+~=!@#$%^&*()qwertyuiop[]asdfghjkl;'"
		"zxcvbnm,./?><MNBVCXZ\":LKJKHGFDSA}{POIUYTREWQ\n\t\r";
	EXPECT_EQ(crc32c_masked(str, 94), 3704040417U);
}

