#include <fstream>
#include <snappy-stream.hpp>

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
	std::ifstream ifile("snappy-file")
	if(!ifile.is_open())
		return;
	snappy::iSnappyStream isnstrm(ofile);
	std::cout << isnstrm.rd_buf();
}

int main(int argc, char * argv[])
{
	write();
	read();
	return 0;
}

