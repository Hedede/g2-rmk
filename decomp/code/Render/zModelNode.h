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

	zCModelNode* parentNode;

	zSTRING name; // e.g. ZS_POS (attachment slot)
	zCVisual *visual;
	zMAT4 transform;
	zVEC3 nodeRotAxis;
	float nodeRotAngle;
	zVEC3 translation;
	zMAT4 trafoObjToWorld;
	zMAT4* nodeTransformList;
	zCModelNodeInst* lastInstNode;

};


struct zCModelNodeInst {
	zCModelNodeInst* parentNode;
	zCModelNode* protoNode;

	zCModel *nodeVIsual;
	zMAT4 trafo;
	zMAT4 trafoObjToCam;
	zTBBox3D bbox;
	zCModelTexAniState *texAniState;
	void **somep;
	// char unk[236];
};
