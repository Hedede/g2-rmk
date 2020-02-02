class zCSphereCoordsCache {
public:
	struct zSAziSphereCoord
	{
		float azi;
		zCArraySort<zSElevSphereCoord> elevs;
	};

	struct zSElevSphereCoord {
		float elev;
		void* unk;
		zCArraySort<zSRangeSphereCoord*> ranges;
	};

	struct zSRangeSphereCoord {
		float range;
		// maybe more
	};

	zCSphereCoordsCache() = default;
	~zCSphereCoordsCache() { DeleteList(); }
	void DeleteList();

	zCArraySort<zSAziSphereCoord*> azis;
};

void zCSphereCoordsCache::DeleteList()
{
	for ( int i = azis.numInArray - 1; i >= 0; --i ) {
		auto& elevs = azis[i]->elevs;

		for ( int j = elevs.numInArray - 1; j >= 0; --j ) {
			auto& ranges = elevs[i]->ranges;

			for ( int k = ranges.numInArray - 1; k >= 0; --k ) {
				delete ranges[k];
				ranges.RemoveIndex( k );
			}

			delete elevs[j];
			elevs.RemoveIndex( j );
		}

		delete azis[i];
		azis.RemoveIndex( i );
	}
}
