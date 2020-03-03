class oCCSManager : public zCCSManager {
	zCLASS_DECLARATION(oCCSManager);
public:
	oCCSManager();
	~oCCSManager() override = default;
	void CreateMessage(int msgType) override;
	void CreateOuMessage() override;
	void CreateCutscenePlayer(zCWorld* ownerWorld) override;
	void CreateProperties() override;
};
