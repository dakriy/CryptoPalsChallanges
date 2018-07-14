#include <cstdint>
#include <cassert>
#include <iostream>
#include <string>
#include <functional>

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

void print_if_good_enough(std::string test, char i)
{
	unsigned int counter = 0;
	for (auto c : test)
	{
		if (c == 32 || c >= 65 && c <= 90 || c >= 97 && c <= 122)
		{
			++counter;
		}
	}
	if (counter >= test.size() * 3.f/4.f)
		std::cout << "Key '0x" << convert_4_bits_to_hex((i & 0xF0) >> 4) << convert_4_bits_to_hex(i & 0x0F) << "' Produced: \"" << test << "\"" << std::endl;
}

int main()
{
	std::string encoded;
	std::cout << "Enter the encoded string:" << std::endl;
	std::cin >> encoded;

	if(encoded.size() % 2 != 0)
	{
		std::cout << "Number of bits to be divisible by 8" << std::endl;
		return 1;
	}
	
	std::cout << "Results:" << std::endl;
	
	for (unsigned int i = 0; i <= UCHAR_MAX; i++)
	{
		std::string decoded;
		for (unsigned int j = 0; j < encoded.size(); j+=2)
		{
			uint8_t dec = convert_hex_to_4_bits(encoded[j + 1]) & 0x0F;
			dec |= convert_hex_to_4_bits(encoded[j]) << 4;
			decoded += static_cast<char>(dec ^ i);
		}
		print_if_good_enough(decoded, i);
	}
	system("pause");
	return 0;
}