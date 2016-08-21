class zCLightning {
	struct zCBolt {
		virtual ~zCBolt();
		virtual void Render(zTRenderContext &);
		virtual void FreeResources();
	};
};
