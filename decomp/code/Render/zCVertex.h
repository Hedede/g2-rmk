class zCVertex {
public:
	static void ResetVertexTransforms()
	{
		s_MemMan.Restore(0);
		auto ptr = s_MemMan.Alloc();
		if (ptr)
			*ptr = 0;
	}
public:
	zVEC3 Position;

	int TransformedIndex;
	int MyIndex;
};
