void zCSession::CamInit(zCVob *theCamVob, zCCamera *theCamera)
{
	this->camera = theCamera;
	camera->SetRenderTarget(viewport);

	this->camVob = theCamVob;
	world->AddVob(camVob);

	camVob->SetCollDetStat(0);
	camVob->SetCollDetDyn(0);
	camVob->SetSleeping(0);
	camVob->SetPhysicsEnabled(0);

	camera->connectedVob = camVob;

	zsound->SetListener(camVob);
}
