#include <cstdint>
#include <cassert>
#include <iostream>
#include <string>

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
	std::string str, key;
	std::cout << "Enter your string:" << std::endl;
	std::cin >> str;
	std::cout << "Enter the key:" << std::endl;
	std::cin >> key;
	if(str.size() != key.size())
	{
		std::cout << "Key and string need to be the same size for a fixed xor." << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

	std::cout << "RESULT: " << std::endl;

	for (size_t i = 0; i < str.size(); i++)
	{
		const uint8_t keybuff = convert_hex_to_4_bits(key[i]),
			buffer = convert_hex_to_4_bits(str[i]),
			res = buffer ^ keybuff;
		std::cout << convert_4_bits_to_hex(res & 0x0F);
	}
	std::cout << std::endl;
	system("pause");
	return 0;
}