#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

#include "HuffmanTree.h"

void openFile(std::string _fileName, int _option);


int main() {
	int input;
	std::string fileName;

	std::cout << "Please select the option (Number):" << std::endl;
	std::cout << "1. Encode file" << std::endl;
	std::cout << "2. Decode file" << std::endl;
	std::cout << "3. Exit" << std::endl;
	while (!(std::cin >> input)) {
		std::cout << "Invalid command. Please try again: ";
		std::cin.clear();
	}
	std::cout << "Please type the name of the file you want to encode or decode:" << std::endl;
	std::cin >> fileName;
	openFile(fileName, input);

	return 0;
}
void openFile(std::string _fileName, int _option) {
	std::ifstream iFile(_fileName.c_str(), std::ios::binary | std::fstream::in);
	std::ofstream wFile;
	if (iFile.is_open()) {
		std::cout << std::endl << "Loading file...";

		std::streampos start = iFile.tellg();
		iFile.seekg(0, std::ios::end);
		std::streampos end = iFile.tellg();
		iFile.seekg(0, std::ios::beg);
		
		unsigned char* buffer = new unsigned char[static_cast<size_t>(end - start)];
		// read data as a block:
		iFile.read((char*)buffer, static_cast<size_t>(end - start));
		
		std::cout << " File loaded successfully." << std::endl;
		unsigned char *pDes;
		int nDesLen;
		//encode or decode

		if (_option == 1)
		{
			std::cout << " Start encoding..." << std::endl;
			CompressHuffman(buffer, static_cast<size_t>(end - start), pDes, nDesLen);
			wFile.open((_fileName + ".Huff").c_str(), std::ofstream::binary);
			wFile.write((char *)pDes, nDesLen);
			std::cout << (_fileName + ".Huff").c_str() << " File created." << std::endl;
			system("pause");
		}
		else if (_option == 2)
		{
			DecompressHuffman(buffer, static_cast<size_t>(end - start), pDes, nDesLen);
			wFile.open(_fileName.substr(0, _fileName.find_last_of(".")).c_str(), std::ofstream::binary);
			wFile.write((char *)pDes, nDesLen);
			std::cout << _fileName.substr(0, _fileName.find_last_of(".")).c_str() << " File created." << std::endl;
			system("pause");
		}
		else
		{
			return;
		}
		delete[] buffer;
		delete pDes;
		iFile.close();
		wFile.close();
	}
	
}

std::string getFileName(std::string _file)
{
	std::string name;
	std::string ext;
	unsigned int dot = _file.find_last_of(".");
	if (dot != std::string::npos)
	{
		name = _file.substr(0, dot);
		ext = _file.substr(dot, _file.size() - dot);
	}
	else
	{
		name = _file;
		ext = "";
	}
	return name;
}


std::string getExtName(std::string _file)
{
	std::string name;
	std::string ext;
	unsigned int dot = _file.find_last_of(".");
	if (dot != std::string::npos)
	{
		name = _file.substr(0, dot);
		ext = _file.substr(dot, _file.size() - dot);
	}
	else
	{
		name = _file;
		ext = "";
	}
	return ext;
}
