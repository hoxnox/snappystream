#include <fstream>
#include <snappystream.hpp>

void write()
{
	std::ofstream ofile("snappy-file");
	if(!ofile.is_open())
		return;
	snappy::oSnappyStream osnstrm(ofile);
	osnstrm << "Hello, world!" << std::endl;
}

void read()
{
	std::ifstream ifile("snappy-file");
	if(!ifile.is_open())
		return;
	snappy::iSnappyStream isnstrm(ifile);
	std::cout << isnstrm.rdbuf();
}

int main(int argc, char * argv[])
{
	write();
	read();
	return 0;
}

