#include <cstdint>
#include <array>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
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

typedef struct string_score
{
	string_score(std::string& en, std::string& de, const int sc, const char k) : decoded_str(de), encoded_str(en), score(sc), key(k) {}
	string_score()
	{
		decoded_str = encoded_str = "";
		score = 0;
		key = 0;
	}
	std::string decoded_str = "";
	std::string encoded_str = "";
	int score = 0;
	char key = 0;
} string_score;

string_score score_string(std::string& decoded, std::string& encoded, const char key)
{
	auto score = 0;
	for (auto chr : decoded)
	{
		chr = tolower(chr);
		if (chr == ' ') // Spaces are worth a lot
			score += 10;
		else if (chr == 'a' || chr == 'e' || chr == 'i' || chr == 'o' || chr == 'u')
			score += 5;
		else
			--score;
	}
	return string_score(encoded, decoded, score, key);
}

int main()
{
	std::string encoded;
	std::ifstream file("strings.txt", std::ifstream::in);
	std::array<string_score, 5> top;
	auto line_num = 0;
	
	while (std::getline(file, encoded))
	{
		++line_num;
		std::cout << "On Line " << line_num << std::endl;
		if (encoded.empty())
			continue;

		for (unsigned int i = 0; i <= UCHAR_MAX; i++)
		{
			std::string decoded;
			for (unsigned int j = 0; j < encoded.size(); j += 2)
			{
				uint8_t dec = convert_hex_to_4_bits(encoded[j + 1]) & 0x0F;
				dec |= convert_hex_to_4_bits(encoded[j]) << 4;
				decoded += static_cast<char>(dec ^ i);
			}
			const auto current_scored_str = score_string(decoded, encoded, i);
			for (auto& scores : top)
			{
				if (current_scored_str.score > scores.score)
				{
					scores = current_scored_str;
					break;
				}
			}
		}
	}

	std::cout << "Top 5 results are:" << std::endl;
	for(auto& scores: top)
		std::cout << scores.encoded_str << " yielded \"" << scores.decoded_str << "\" with a score of " << scores.score << " and a key of '0x" << convert_4_bits_to_hex((scores.key & 0xF0) >> 4) << convert_4_bits_to_hex(scores.key & 0x0F) << "'" << std::endl << std::endl;

	system("pause");
	return 0;
}