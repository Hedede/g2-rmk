class oCVob : public zCVob {
	Z_OBJECT(oCVob);
public:
	static void ShowVobDebugInfo(zCCamera* camera)
	{
		for (oCVob* vob : debugList)
			vob->ShowDebugInfo(camera);
	}

	static void ClearDebugList()
	{
		debugList.DeleteList(); // don't remember actual func
	}

	oCVob() = default;
	virtual ~oCVob() = default;

	virtual void Archive(zCArchiver& arc)
	{
		zCVob::Archive(arc);
	}
	virtual void Unarchive(zCArchiver& arc)
	{
		zCVob::Unarchive(arc);
	}


	virtual void Render(zTRenderContext &);
	virtual int GetCSStateFlags()
	{
		return 0;
	}

	virtual void Init() { }

	virtual void ShowDebugInfo(zCCamera *);

	void ToggleShowDebug()
	{
		if (GetShowDebug()) {
			debugList.Remove(this);
		} else {
			debugList.Insert(this);
		}
	}

	void SetShowDebug(int b)
	{
		if (b) {
			if (!GetShowDebug())
				debugList.Insert(this);
		} else {
			if (GetShowDebug())
				debugList.Remove(this);
		}
	}

	bool GetShowDebug() const
	{
		for (oCVob* vob : debugList)
			if (vob == this)
				return true;
		return false;
	}

	virtual int GetInstance()
	{
		return -1;
	}

	virtual zSTRING GetInstanceName()
	{
		return "";
	}
	virtual bool IsOwnedByGuild(int id)
	{
		return false;
	}
	virtual bool IsOwnedByNpc(int instance)
	{
		return false;
	}
	virtual int DoFocusCheckBBox()
	{
		return 0;
	}
	virtual oCAIVobMove* GetAIVobMove()
	{
		if (auto ai = zDYNAMIC_CAST<oCAIVobMove>(callback_ai))
			return ai;

		auto ai = new oCAIVobMove;
		SetAI(ai);
		ai->Release();
	}

	virtual void GetSoundMaterial_AM(zCSoundManager::zTSndManMedium& med,oTSndMaterial& mat, int damage);
	virtual void SetSoundMaterial(oTSndMaterial mat) { }
	virtual oTSndMaterial GetSoundMaterial()
	{
		return 0;
	}

	bool GetFloorPosition(zVEC3& floorPos);
	bool GetFloorPositionWithVob(zVEC3& floorPos, zCVob*& withVob)
	{
		withVob = nullptr;

		bool result = GetFloorPosition(floorPos);

		if (result)
			withVob = homeWorld->foundVob;

		return result;
	}

	void SetOnFloor(zVEC3& centerPos)
	{
		zVEC3 floorPos = centerPos;
		if ( GetFloorPosition(floorPos) )
			SetPositionWorld(floorPos);
	}

	bool HasEnoughSpace(zVEC3& newPos);
};

bool oCVob::GetFloorPosition(zVEC3& floorPos)
{
	if (!homeWorld)
		return false;

	float suspension = GetPositionWorld().y - bbox3D.mins.y;

	zVEC3 ray {0, -1000.0, 0};

	homeWorld->TraceRayNearestHit(floorPos, ray, this, 0x21);

	if (homeWorld->foundPoly || homeWorld->foundVob) {
		floorPos = homeWorld->foundIntersection;
		floorPos.y += suspension + 4.0;
		return true;
	}

	return false;
}

int oCVob::HasEnoughSpace(zVEC3& vec)
{
	if ( homeWorld ) {
		auto old_col_class = collisionObjectClass;

		SetCollisionClass(zCCollObjectComplex::s_oCollObjClass);
		SetCollDetStat(0);
		SetCollDetDyn(0);

		zMAT4 curTrafo = trafoObjToWorld;
		zMAT4 newTrafo = trafoObjToWorld;
		newTrafo.SetTranslation(vec);

		auto free = !DetectCollision(newTrafo);
		SetCollDetStat(0);
		SetCollDetDyn(0);
		SetTrafoObjToWorld(curTrafo);
		SetCollisionClass(old_col_class);
		return free;
	}

	zWARNING("U: OVOB: Space-Test failed : Vob not in World."); // 381,_ulf
	return false;
}
