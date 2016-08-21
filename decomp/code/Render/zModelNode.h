struct zCModelNode {
	void SetNodeVisualS(zCVisual* visual)
	{
		if (this->visual != visual) {
			Release(visual);
			if (visual)
				visual->AddRef();
			this->visual = visual;
		}
	}

	void *unk;
	zSTRING name; // e.g. ZS_POS (attachment slot)

	zCVisual* visual;

	zMAT4 __maru;
	zVEC3 __posmb;
	int __intmh;
	zVEC3 __whaaa;
	zMAT4 __anudaermat;
	int __also[2];
};

struct zCModelNodeInst {
	void *_somep;
	whatels *name;
	zCModel *_model;
	zMAT4 mat1;
	zMAT4 trafoWorld;
	zTBBox3D bbox;
	zCModelTexAniState *texanistate;
	void **somep;
	// char unk[236];
};
