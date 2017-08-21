class oCCSManager : public zCCSManager {
	Z_OBJECT(oCCSManager);
public:
	virtual ~oCCSManager();
	virtual void CreateMessage(int);
	virtual void CreateOuMessage();
	virtual void CreateCutscenePlayer(zCWorld *);
	virtual void CreateProperties();
};

//------------------------------------------------------------------------------
// _Andre/oCSManager.cpp
