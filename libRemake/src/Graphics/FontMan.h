#ifndef G2Remake_FontMan_H
#define G2Remake_FontMan_H
#include <vector>
#include <memory>
#include <Gothic/Graphics/zFont.h>
#include <Gothic/Graphics/zFontMan.h>

namespace g2 {
struct FontMan {
	FontMan()  = default;
	~FontMan() = default;

	FontMan(FontMan&&) = default;
	FontMan& operator=(FontMan&&) = default;

	FontMan(FontMan const&) = delete;
	FontMan& operator=(FontMan const&) = delete;

	// TODO: separate font loader
	size_t Load(std::string const& name);
	zCFont* GetFont(size_t num)
	{
		if (num < fontList.size())
			return fontList[num].get();
		return nullptr;
	}

private:
	std::vector<std::unique_ptr<zCFont>> fontList;
};
} // namespace g2
#endif//G2Remake_FontMan_H
