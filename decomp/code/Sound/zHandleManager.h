struct zCHandleManager {
	zCHandleManager() = default;
	void DisposeAllSamples()
	{
		for (auto handle : handle3dList) {
			if (handle && AIL_3D_sample_status(handle) != 1) {
				AIL_end_3D_sample(handle);
				AIL_release_3D_sample_handle(handle);
			}
		}

		handle3dList.DeleteList();

		for (auto sample : sampleList) {
			if (sample && AIL_sample_status(sample) != 1) {
				AIL_stop_sample(sample);
				AIL_release_sample_handle(sample);
				while ( AIL_sample_status(sample) != 1 );
			}
		}

		sampleList.DeleteList();
	}


	zCArray<h3DPOBJECT*> handle3dList;
	zCArray<_SAMPLE*> sampleList;
};
