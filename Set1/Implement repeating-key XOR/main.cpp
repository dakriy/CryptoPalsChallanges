#include <cstdint>
#include <cassert>
#include <iostream>
#include <string>
#include <functional>
#include <fstream>

char convert_4_bits_to_hex(uint8_t bits)
{
	bits &= 0x0F;
	if (bits < 10)
		return '0' + bits;
	if (bits < 16)
		return 'a' + bits - 10;
	// Should never get here
	assert(false);
	return 'x';
}

uint8_t convert_hex_to_4_bits(char hex)
{
	if (hex <= '9')
		return hex - '0';
	return hex - 'a' + 10;
}

int main()
{
	std::string file_name, key;
	std::cout << "Enter tbe filename to encrypt/decrypt:" << std::endl;
	std::cin >> file_name;
	std::cout << "Enter in the key to encrypt/decrypt it with:" << std::endl;
	std::cin >> key;
	std::fstream fs;
	fs.open(file_name, std::fstream::in | std::fstream::binary);
	if (!fs.is_open())
	{
		std::cout << "Could not open the file." << std::endl;
		system("pause");
		return 0;
	}

	char * buffer;
	int length;
	//std::string hex;

	if (fs)
	{
		fs.seekg(0, std::fstream::end);
		length = fs.tellg();
		fs.seekg(0, std::fstream::beg);

		buffer = new char[length];

		fs.read(buffer, length);
		
		fs.close();
	}
	else
	{
		std::cout << "Somethng went wrong." << std::endl;
		system("pause");
		return 0;
	}
	
	for (auto i = 0; i < length; i++)
	{
		buffer[i] ^= key[i % key.length()];
		//hex += convert_4_bits_to_hex((byte & 0xF0) >> 4);
		//hex += convert_4_bits_to_hex(byte & 0x0F);
	}

	file_name.append(".xor");

	fs.open(file_name, std::fstream::out | std::fstream::trunc);

	if (!fs.is_open())
	{
		std::cout << "Could not open the file." << std::endl;
		system("pause");
		return 0;
	}

	if (fs)
	{
		fs.write(buffer, length);
	}

	std::cout << "DONE" << std::endl;
	system("pause");
	return 0;
}