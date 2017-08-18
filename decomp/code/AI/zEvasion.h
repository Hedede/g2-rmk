
//Don't know what actual name is
using FindFunc = int(zSEvasionSearchDesc const &);

struct zSEvasionSearchDesc {
	zSEvasionSearchDesc() = default;
	~zSEvasionSearchDesc() = default;
	zSEvasionSearchDesc& operator=(zSEvasionSearchDesc const&) = default;

	struct zSEvasionSearchOptions {
		zSEvasionSearchOptions() = default;
		zSEvasionSearchOptions& operator=(zSEvasionSearchOptions const&) = default;

		zVEC3 vec0;
		zVEC3 vec1;
		zVEC3 vec2;
		zVEC3 vec3;
		zSTRING str0;
		zSTRING str1;
	}; // 0x58

	FindFunc* searchFunc;
	zSTRING   searchString;
	zCArray<zSEvasionSearchOptions> searchOptions;
}; //0x24


// Interesting specimen (not found yet where it is used:)
zCArray<FindFunc**>;

