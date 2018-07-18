//#define USEPOPCNT
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include <utility>
#include <iostream>
#ifdef USEPOPCNT
#include <intrin.h>
#else
#include <bitset>
unsigned __popcnt(const unsigned data)
{
	std::bitset<32> b(data);
	return b.count();
}
#endif
#define MAX_KEYSIZE 40

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static bool is_base64(char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

// returns UINT_MAX if the strings are not the same length
unsigned compute_hamming_distance(const std::string& str1, const std::string& str2)
{
	if (str1.size() != str2.size())
		return UINT_MAX;
	unsigned count = 0;
	for (unsigned i = 0; i < str1.size(); i++)
		count += __popcnt(static_cast<unsigned>(str1[i] ^ str2[i]));
	return count;
}

unsigned compute_hamming_distance(const unsigned uno, const unsigned dos)
{
	return __popcnt(uno ^ dos);
}

unsigned compute_hamming_distance(const char * str1, const char * str2, const unsigned length)
{
	unsigned count = 0;
	for (unsigned i = 0; i < length; i++)
		count += __popcnt(static_cast<unsigned>(str1[i] ^ str2[i]));
	return count;
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

std::vector<char> base64_decode(std::string const& encoded_string) {
	auto in_len = encoded_string.size();
	auto i = 0;
	int j;
	auto in_ = 0;
	char char_array_4[4], char_array_3[3];
	std::vector<char> ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret.push_back(char_array_3[i]);
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
	}

	return ret;
}

typedef struct string_score
{
	string_score(std::string& de, const int sc, const char k) : decoded_str(de), score(sc), key(k) {}
	string_score(std::string& de, const char k) : decoded_str(de), key(k)
	{
		score_string();
	}
	string_score() = default;
	int score_string()
	{
		auto s = 0;
		for (auto chr : decoded_str)
		{
			chr = tolower(chr);
			if (chr >= 'a' && chr <= 'z' || chr == ' ')
				++s;
			else
				s -= 5;
		}
		return score = s;
	}
	std::string decoded_str = "";
	int score = INT_MIN;
	char key = 0;
} string_score;

string_score break_single_character_xor(std::vector<char> data)
{
	string_score top;
	for (unsigned int i = 0; i <= UCHAR_MAX; i++)
	{
		std::string decoded;
		for (auto j : data)
			decoded += static_cast<char>(j ^ i);
		const string_score current_scored_str(decoded, i);
		if (current_scored_str.score > top.score)
			top = current_scored_str;
	}
	return top;
}

// basically base^x
unsigned intpow(const unsigned base, const unsigned x)
{
	auto retval = base;
	if (x == 0)
		return 1;
	for (unsigned i = 0; i < x - 1; i++)
		retval *= base;
	return retval;
}

int main()
{
	std::ifstream f_h("6.txt", std::ios::in);
	std::string line;
	std::vector<char> data;
	while(std::getline(f_h, line))
	{
		auto decoded_line = base64_decode(line);
		data.insert(data.end(), decoded_line.begin(), decoded_line.end());
	}

	// First: Key size.
	// Second: Normalized hamming distance.
	std::array<std::pair<unsigned, unsigned>, 3> keysizes = { std::make_pair(0, UINT_MAX), std::make_pair(0, UINT_MAX) , std::make_pair(0, UINT_MAX) };

	for (unsigned i = 2; i < MAX_KEYSIZE; i++)
	{
		// Key size larger than data doesn't make sense because then it wouldn't repeat.
		if (data.size() < i)
			continue;
		const auto dist = compute_hamming_distance(&data[0], &data[i], i) / i;
		for (auto& keysize : keysizes)
		{
			if (dist < keysize.second)
			{
				keysize.first = i;
				keysize.second = dist;
				break;
			}
		}
	}

	for (const auto & keysize : keysizes)
	{
		// Put each key's character in its own block
		std::vector<std::vector<char>> blocks(keysize.first);
		for (unsigned i = 0; i < data.size(); ++i)
			blocks[i % keysize.first].push_back(data[i]);

		std::vector<string_score> decoded_blocks;

		auto score = 0;

		for (const auto & block : blocks)
		{
			const auto t = break_single_character_xor(block);
			decoded_blocks.push_back(t);
			score += t.score;
		}

		std::cout << "Best guess:" << std::endl;
		std::cout << "Score: " << score << std::endl;
		std::cout << "Key: '";
		for (const auto & decoded_block : decoded_blocks)
		{
			std::cout << decoded_block.key;
		}
		std::cout << "'" << std::endl;
		std::cout << "Content: " << std::endl;

		for (unsigned i = 0; i < data.size(); ++i)
			std::cout << decoded_blocks[i % decoded_blocks.size()].decoded_str[i / keysize.first];

		std::cout << std::endl << std::endl;
	}

	system("pause");
	return 0;
}