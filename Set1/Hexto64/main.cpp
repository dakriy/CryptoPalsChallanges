#include <cstdint>
#include <iostream>
#include <string>
#include <cassert>

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

char convert_byte_to_base64(uint8_t byte)
{
	// Make sure it's a good value
	byte &= 0x3F;
	if (byte < 26)
		return 'A' + byte;
	if (byte < 52)
		return 'a' + byte - 26;
	if (byte < 62)
		return '0' + byte - 52;
	if (byte == 62)
		return '+';
	if (byte == 63)
		return '/';
	return '=';
}


int main()
{
	std::string hexInput, output;
	std::cout << "Input your hex string:" << std::endl;
	std::cin >> hexInput;
	// 4 goes into 12, 3 times.
	// 6 goes into 12, 2 times.
	// 3 hex characters -> 2 base64 characters
	uint16_t buffer = 0;
	for (unsigned int i = 0; i < hexInput.size(); i++)
	{
		buffer |= convert_hex_to_4_bits(hexInput[i]) << (2 - i % 3) * 4;

		// Get next base64 character
		if (2 - i % 3 == 0 || i == hexInput.size() - 1)
		{
			for (unsigned int j = 0; j < 2; j++)
			{
				// Start from end
				if (j == 1 && hexInput.size() * 4 % 6 != 0 && i == hexInput.size() - 1)
					std::cout << '=' << std::endl;
				else
					std::cout << convert_byte_to_base64((buffer & (0x3F << (1 - j) * 6)) >> (1 - j)*6);
			}
			buffer = 0;
		}
	}

	return 0;
}