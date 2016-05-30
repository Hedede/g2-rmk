struct zCBBox3DSorterBase {
	static zCBBox3DSorterBase* s_currentSorter;

	zCBBox3DSorterBase()
	{
		for (auto& list : nodeList)
			list.compare = zCBBox3DSorterBase::ArrayCompare;
	}

	virtual ~zCBBox3DSorterBase();
	virtual zTBbox3D GetBBox(zTNode const*);

	void RemoveHandle(zTBoxSortHandle* handle)
	{
		handles.Remove(handle);
	}

	void Sort() {
		s_currentSorter = this;

		sorted = 1;
		for (unsigned i = 0; i < zCWorld_DIMENSION; ++i) {
			s_currentDimension = i;
			nodeList[i].QSort();
		}
	}

	struct zTNode {

	};

	struct zTBoxSortHandle  {
		virtual ~zTBoxSortHandle()
		{
			if (mySorter)
				mySorter.RemoveHandle(this);
			ClearActive();
		}

		virtual void AddActive(void*) = 0;
		virtual void DelActive(void*) = 0;
		virtual void ClearActive()    = 0;


		zCBBox3DSorterBase* mySorter = nullptr;
		zTBBox3D bbox3D;

		int indexBegin[zCWorld_DIMENSION] = {};
		int indexEnd  [zCWorld_DIMENSION] = {};
	};

	zCArray<zTBoxSortHandle *> handles;
	zCArraySort<zTNode*> nodeList[zCWorld_DIMENSION];

	zBOOL sorted;
};

template<typename T>
struct zCVobBBox3DSorter : zCBBox3DSorterBase {
	virtual ~zCVobBBox3DSorter() = default;

	struct zTBoxSortHandle : zCBBox3DSorterBase::zTBoxSortHandle {
		virtual ~zTBoxSortHandle() = default;

		void AddActive(void* data) override
		{
			activeList.Insert((T*)data);
		}

		void DelActive(void* data) override
		{
			activeList.Remove((T*)data);
		}

		void ClearActive() override
		{
			activeList.DeleteList();
		}

		zCArray<T*> activeList;
	};
};
