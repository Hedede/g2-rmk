#include <cstddef>
namespace std {
template<class T>
struct allocator {
};
class string {
public:
	string()
	{
	}
	string(string& other)
	{
	}
	string(char const* begin)
	{
	}
	string(char const* begin, char& end)
	{
	}
	string(size_t,char)
	{
	}
	~string()
	{
		delete[] arr;
	}
	int find(char const*, unsigned int, unsigned int)
	{
		return 0;}
	int compare(char const*) {
		return 0;
	}

	int compare(std::string) {
		return 0;
	}

	int compare(std::string*) {
		return 0;
	}

	std::string& assign(char const*)
	{
		return * this;
	}

	std::string assign(string&)
	{
		return * this;
	}

	int _Refcnt(int a)
	{
		return a;
	}
	
	std::string& replace(std::string other)
	{
		return other;
	}

	std::string& assign(string&, int, int)
	{
		return *this;
	}

	std::string& assign(void const* ptr, int size)
	{
		return *this;
	}

	char& at(unsigned int)
	{
		return *arr;
	}

	void _Tidy(bool a2)
	{
	}

	bool _Grow(size_t s, bool)
	{
	}

	int _Eos(int){}

	std::string& erase(size_t p1, size_t p2)
	{
		return *this;
	}

private:
	char* arr;
};
}

int main() {
	std::string poebenzia(100,'c');
	std::string str;
	std::string str2;
	str.compare(str2);
	str.compare(&str2);
	str._Grow(1,1);
	str._Eos(1);
	str.erase(0,900);
	str.assign("ooo");
	str.replace(str2);
	str.assign(str2, 0,0);
	str.assign((void*)0, 0);
	str.at(0) = str.at(2);			//int 
	std::string str3(str);
	char c;
	str.find("z",0,1);
	std::string str5("aaa");
	return str.compare("aaa") + str._Refcnt(2);
}
