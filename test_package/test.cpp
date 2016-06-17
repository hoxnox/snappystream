#include <fstream>
#include <string>
#include <snappystream.hpp>

int write()
{
	std::ofstream ofile("snappy-file");
	if(!ofile.is_open())
		return -1;
	snappy::oSnappyStream osnstrm(ofile);
	osnstrm << "Hello!" << std::endl;
	return 0;
}

int read()
{
	std::ifstream ifile("snappy-file");
	if(!ifile.is_open())
		return -1;
	snappy::iSnappyStream isnstrm(ifile);
	std::string str;
	isnstrm >> str;
	if (str != "Hello!")
		return -1;
	return 0;
}

int main(int argc, char * argv[])
{
	try
	{
		if (write() == -1)
			return -1;
		if (read() == -1)
			return -1;
	}
	catch(...)
	{
		return -1;
	}
	return 0;
}

