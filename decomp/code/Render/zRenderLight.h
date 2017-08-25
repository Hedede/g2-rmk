struct zCRenderLight {
	zCRenderLight() = default;
	
	char unk[76];
};

struct zCRenderLightContainer {
	zCRenderLight lights[8];
	int unk[4];
};
