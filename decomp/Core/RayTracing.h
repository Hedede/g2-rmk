struct zCRayTurboAdmin {
	static int s_iPolyTreshold;
	static int CanHaveSpatialInfo(zCBspNode const* node)
	{
		if ( s_iPolyTreshold > 0 )
			return node->numPolys > s_iPolyTreshold;
		return false;
	}
};
