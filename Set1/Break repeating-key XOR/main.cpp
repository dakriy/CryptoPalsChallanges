#include <string>
#include <iostream>
#include <intrin.h>

// returns UINT_MAX if the strings are not the same length
unsigned compute_hamming_distance(std::string& str1, std::string& str2)
{
	if (str1.length() != str2.length())
		return UINT_MAX;
	unsigned count = 0;
	for (unsigned i = 0; i < str1.length(); i++)
		count += __popcnt(static_cast<unsigned>(str1[i] ^ str2[i]));
	return count;
}

int main()
{
	std::string str1 = "this is a test";
	std::string str2 = "wokka wokka!!!";
	std::cout << compute_hamming_distance(str1, str2) << std::endl;
	system("pause");
	return 0;
}