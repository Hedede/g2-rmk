void  oCNpc::RbtInit(zVEC3& tpos, zCVob* tvob)
	{
		RbtReset();
		RbtUpdate(tpos, tvob);
	}

// private
zBOOL oCNpc::RbtCheckForSolution(zVEC3 curPos, zVEC3 targetPos)
{
	auto diff = targetPos - curPos;
	diff.y = 0.0;

	auto distApprox = diff.LengthApprox()
	if ( distApprox * distApprox > rbt.maxTargetDist )
		return 1;

	auto bbox = bbox3D;

	if ( bbox.min.y <= targetPos.y && bbox.max.y >= targetPos.y )
		return 1;

	rbt.failurePossibility+ = ztimer.frameTimeFloat / 30.0;

	return 0;
}
