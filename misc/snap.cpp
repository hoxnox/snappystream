/**@author Merder Kim <hoxnox@gmail.com>
 * @date 20130123 09:40:29 
 * @brief Simplest SnappyStream based cmd archiver*/

#include <iostream>
#include <sstream>
#include <snappystream.hpp>

using namespace snappy;

int main(int argc, char *argv[])
{
	std::ios::sync_with_stdio(false);
	if(argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " x|c" << std::endl;
		return 1;
	}
	if(argv[1][0] == 'x')
		std::cout << iSnappyStream(std::cin).rdbuf();
	else if(argv[1][0] == 'c')
		oSnappyStream(std::cout) << std::cin.rdbuf();
	return 0;
}

