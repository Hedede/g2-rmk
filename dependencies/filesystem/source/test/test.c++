#include <filesystem>

#include <iostream>
int main()
{
	std::error_code ec;
	std::cout << awstd::filesystem::exists("test.c++", ec) << '\n';
	std::cout << awstd::filesystem::exists("main.c++", ec) << '\n';
}
