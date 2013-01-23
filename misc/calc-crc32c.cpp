#include <crc32c.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <iostream>


int main(int argc, char * argv[])
{
	int fdin, fdout;
	char *src, *dst;
	struct stat statbuf;

	if(argc < 2)     
	{
		std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
		return 1;
	}

	if((fdin = open(argv[1], O_RDONLY)) < 0)
	{
		std::cerr << "Error file opening" << std::endl;
		return 1;
	}

	if(fstat(fdin, &statbuf) < 0)
	{
		std::cerr << "Error getting file info" << std::endl;
		return 1;
	}

	if((src = static_cast<char*>(mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0))) == (caddr_t) - 1)
	{
		std::cerr << "Error file mapping" << std::endl;
		return 1;
	}

	std::cout << std::hex << crc32c_masked(src, statbuf.st_size) << std::endl;

	munmap(&src, statbuf.st_size);

	return 0;
}

