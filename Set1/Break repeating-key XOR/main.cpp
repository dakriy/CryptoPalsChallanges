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
	string_score(std::string& en, std::string& de, const int sc, const char k) : decoded_str(de), encoded_str(en), score(sc), key(k) {}
	string_score(std::string& de, const int sc, const char k) : decoded_str(de), score(sc), key(k) {}
	string_score()
	{
		decoded_str = encoded_str = "";
		score = INT_MIN;
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
		if (chr == ' ')
			score += 5;
		else if (chr == 'a' || chr == 'e' || chr == 'i' || chr == 'o' || chr == 'u')
			score += 5;
		else
			--score;
	}
	return string_score(encoded, decoded, score, key);
}

string_score score_string(std::string& decoded, const char key)
{
	auto score = 0;
	for (auto chr : decoded)
	{
		chr = tolower(chr);
		if (chr == ' ')
			score += 5;
		else if (chr == 'a' || chr == 'e' || chr == 'i' || chr == 'o' || chr == 'u')
			score += 5;
		else
			--score;
	}
	return string_score(decoded, score, key);
}

string_score break_single_character_xor(std::vector<char> data)
{
	// key pair score.
	string_score keypairscore;
	for (unsigned int i = 0; i <= UCHAR_MAX; i++)
	{
		std::string decoded;
		for (auto j : data)
		{
			decoded += static_cast<char>(j ^ i);
		}
		const auto current_scored_str = score_string(decoded, i);
		if (current_scored_str.score > keypairscore.score)
		{
			keypairscore = current_scored_str;
		}
	}
	return keypairscore;
}

int main()
{
	std::cout << compute_hamming_distance("this is a test", "wokka wokka!!!") << std::endl;
	std::ifstream f_h("6.txt", std::ifstream::in);
	std::string line;
	std::vector<char> data;
	while(std::getline(f_h, line))
	{
		auto decoded_line = base64_decode(line);
		data.insert(data.end(), decoded_line.begin(), decoded_line.end());
	}

	// First element is the key size, second element is the normalized hamming distance
	std::array<std::pair<unsigned, unsigned>, 3> keysizes = { std::make_pair(0, UINT_MAX), std::make_pair(0, UINT_MAX) , std::make_pair(0, UINT_MAX) };

	for (auto i = 2; i < MAX_KEYSIZE; i++)
	{
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

	// Store each matching key character in the same buffer
	for (const auto & keysize : keysizes)
	{
		if (keysize.first == 0)
			continue;
		std::vector<std::pair<string_score, std::vector<char>>> transposed(keysize.first);
		for (unsigned i = 0; i < data.size(); ++i)
			transposed[i % keysize.first].second.push_back(data[i]);

		std::string key;

		for(auto & block : transposed)
		{
			block.first = break_single_character_xor(block.second);
			key += block.first.key;
		}

		std::cout << "Guess:" << std::endl;
		std::cout << "Key guess:" << std::endl << key << std::endl;
		std::cout << "Decrypted:" << std::endl;
		for (unsigned i = 0; i < data.size(); ++i)
			std::cout << transposed[i % keysize.first].first.decoded_str[i / keysize.first];

		std::cout << std::endl << std::endl;
	}

	system("pause");
	return 0;
}