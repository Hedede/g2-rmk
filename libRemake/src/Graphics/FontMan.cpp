#include <Graphics/FontMan.h>
#include <Logging/Log.h>
#include <aw/utility/string/case.h>

namespace g2 {
size_t FontMan::Load(std::string  name)
{
	aw::string::tolower(name);

	size_t idx = 0;
	for (; idx < fontList.size(); ++idx) {
		auto fontName = fontList[idx]->GetFontName();
		aw::string::tolower(fontName);
		if (fontName == name)
			break;
	}

	if (idx == fontList.size())
		fontList.emplace_back(new zCFont{name});

	return idx;
}
} // namespace g2
