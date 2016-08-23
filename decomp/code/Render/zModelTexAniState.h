struct zCModelTexAniState {
	zCModelTexAniState() = default;
	~zCModelTexAniState() { DeleteTexList(); }
	void DeleteTexList()
	{
		delete[] textures;
		textures = 0;
		numTextures = 0;
	}


	int numTextures = 0;
	zCTexture **textures = 0;
	int varNr[2][4];
	int alphaTestingEnabled = 0;
};

int zCModelTexAniState::FinishTexList(zCArray<zCTexture*>& texList)
{
	delete[] textures;
	textures = 0;
	numTextures = 0;

	if ( texList.GetNum() > 4 ) {
		zFATAL("D: zModel(zCModelTexAniState::BuildTexListFromMesh): Too many textures in meshLib.. ");
		texList.Resize(4);
	}

	numTextures = texList->numInArray;
	if ( numTextures > 0 ) {
		textures = new (zCTexture*)[numTextures]; // _Dieter\\zModel.cpp", 1100
		memcpy(textures, texList.Data(), sizeof(void*) * numTextures);
	}

	return numTextures;
}

void zCModelTexAniState::SetChannelVariation(channel, int varNr, zSTRING* texNamePart)
{
	if ( channel < 2 ) {
		for ( auto i = 0; i < numTextures; ++i ) {
			if ( texNamePart ) {
				auto& texName = textures[i]->GetObjectName();
				if (texName.Search(0,*texNamePart,1) == -1)
					continue;
			}
			varNr[channel][i] = varNr;
		}
	}
}
