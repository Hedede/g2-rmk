#include <Graphics/FontMan.h>
#include <Logging/Log.h>

namespace g2 {
size_t FontMan::Load(std::string const& name)
{
	size_t idx = 0;
	for (; idx < fontList.size(); ++idx) {
		if (fontList[idx]->GetFontName() == name)
			break;
	}

	if (idx == fontList.size())
		fontList.emplace_back(new zCFont{name});

	return idx;
}
} // namespace g2
