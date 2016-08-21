class zCFlash {
	class zCBolt {
		virtual ~zCBolt();
		virtual void Render(zTRenderContext &);
		virtual void FreeResources();
	};
	virtual ~zCFlash();
	virtual void Render(zTRenderContext&);
	virtual void IsBBox3DLocal();
	virtual void GetBBox3D();
	virtual void GetVisualName();
	virtual void GetVisualDied();
	virtual void SetVisualUsedBy(zCVob *);
	virtual void DynLightVisual(zCArray<zCVobLight *> const	&,zMAT4	*);
};
