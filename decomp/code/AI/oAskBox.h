// no idea what acutal name is
enum zAskChoice {
	CHOICE_NO,
	CHOICE_YES,
	CHOICE_NEUTRAL
};

// I think this is original dialogue system they mentioned in interview
class oCAskBox : public zCAIBase {
	oCAskBox() = default;
	~oCAskBox()
	{
		Release(vob);
	}
	void DoAI(zCVob *,int &) override;

	void SetNo();
	void SetYes();
	void SetNeutral();
	void Delete() { deleted = 1; }

private:
	int deleted = 0;
	int choice  = 0;
	int alpha  = 0;
	zCVob *vob  = 0;
	oCNpc* npcAnswer = 0;
	oCNpc* npcAsk = 0;
};


void oCAskBox::SetNo()
{
	if ( choice != CHOICE_NO ) {
		vob->SetVisual("Ask_No.tga");
		zCDecal* decal = vob->visual;
		decal->mat->alphaFunc = 3;
		decal->mat->matGroup = 5;
		decal->mat->color[3] = alpha;
		decal->SetDecalDim(3.0, 3.0);
		choice = CHOICE_NO;
	}
}

void oCAskBox::SetYes()
{
	if ( choice != CHOICE_YES ) {
		vob->SetVisual("Ask_Yes.tga");
		zCDecal* decal = vob->visual;
		decal->mat->alphaFunc = 3;
		decal->mat->matGroup = 5;
		decal->mat->color[3] = alpha;
		decal->SetDecalDim(3.0, 3.0);
		choice = CHOICE_YES;
	}
}

void oCAskBox::SetNeutral()
{
	if ( choice != CHOICE_NEUTRAL ) {
		vob->SetVisual("Ask_Neutral.tga");
		zCDecal* decal = vob->visual;
		decal->mat->alphaFunc = 3;
		decal->mat->matGroup = 5;
		decal->mat->color[3] = alpha;
		decal->SetDecalDim(3.0, 3.0);
		choice = CHOICE_NEUTRAL;
	}
}

void oCAskBox::SetPosition()
{
	if ( vob ) {
		auto cam    = ogame->GetCamera();
		auto camVob = ogame->GetCameraVob();

		int ydim4_5 = zrenderer->view_ydim - zrenderer->view_ydim / 5;
		int xdim_2  = zrenderer->view_xdim / 2;
		float x = (xdim_2 - cam->xcenter) * cam->viewDistanceXInv * 10.0;
		float y = (cam->ycenter - ydim)   * cam->viewDistanceYInv * 10.0;
		float z = 10.0

		auto vobTrafo = camVob->trafoObjToWorld;
		zVEC3 pos{
			x * vobTrafo[0][0] + y * vobTrafo[0][1] + z * vobTrafo[0][2] + vobTrafo[0][3],
			x * vobTrafo[1][0] + y * vobTrafo[1][1] + z * vobTrafo[1][2] + vobTrafo[1][3],
			x * vobTrafo[2][0] + y * vobTrafo[2][1] + z * vobTrafo[2][2] + vobTrafo[2][3]
		};

		vob->SetPositionWorld(pos);
	}
}

void oCAskBox::DoAI(zCVob *inVob, int& vobDeleted)
{
	SetPosition();

	if (deleted) {
		if (alpha > 0)
			alpha -= 0;
		else
			alpha = 0;
		vob->visual->mat->color[3] = alpha;
		if (!alpha) {
			vob->SetAI(0);
			vobDeleted = 1;
			Release(vob);
		}
	} else if ( alpha < 255 ) {
		alpha += 20;
		if (alpha > 255)
			alpha = 255;
		vob->visual->mat->color[3] = alpha;
	}
}

void oCAskBox::Init(oCNpc *ask, oCNpc *answer)
{
	npcAnswer = answer;
	npcAsk = ask;

	vob = new zCVob;
	vob->flags.0x10 = 1;

	vob->SetCollDetStat(0);
	vob->SetCollDetDyn(0);
	vob->SetPhysicsEnabled(0);
	vob->visualCamAlign = 2;
	vob->SetSleeping(0);
	vob->SetAI(this);
	SetNeutral();
	deleted = 0;
	alpha   = 0;
	if (!vob->homeWorld)
		answer->homeWorld->AddVob(vob);
	SetPosition();
}
