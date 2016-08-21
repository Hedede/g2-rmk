void SafeRelease(zCVob *vob, int wasVobTree)
{
	if ( vob ) {
		int refCtr_Before = vob->refCtr;

		if ( wasVobTree )
			vob->ReleaseVobSubtree(0);

		if ( vob->refCtr == refCtr_Before )
			vob->Release();
	}
}

void SafeAddRef(zCVob *vob)
{
	int refCtr_Before = vob->refCtr;
	vob->AddRefVobSubtree(vob, 0, 1);

	if ( vob->refCtr == refCtr_Before )
		vob->AddRef();
}
