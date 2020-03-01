// P:\\dev\\g2addon\\release\\Spacer\\_bert\\SpacerView.cpp
void CSpacerView::VobApplyPhysics(zCVob *vob, zVEC3 vec)
{
	doc->SetDirty(true);

	auto world = doc->GetWorld(0);

	if ( !vob || vob.flags1.staticVob)
		return;

	if ( world && world->compiled && !world->compiledEditorMode )
	{
		if (dynamic_cast<zCVobWaypoint*>(vob))
		{
			auto wayNet = world->wayNet;
			if (auto waypoint = wayNet->SearchWaypoint(dynamic_cast<zCVobWaypoint*>(vob)))
			{
				waypoint->CorrectHeight(doc->GetWorld(0));
			}
		}
		else
		{
			if (auto npc = dynamic_cast<oCNpc*>(vob))
			{
				zINFO(6,  "B: SPC: Use StandAI for npc " + npc->GetName(1) + " (View::ApplyPhysics)"); // 3060
				npc->UseStandAI();
				npc->SetPhysicsEnabled(1);
			}
			else if ( vob->flags1.collDetectionStatic & vob->GetRigidBody() )
			{
				auto dc = CDC::FromHandle(GetDC(hWnd));

				auto pos = vob->GetTranslationWorld();

				auto physicsEnabled = vob->flags1.physicsEnabled;
				vob->SetSleeping(0);
				vob->SetPhysicsEnabled(1);
				if ( zinput->KeyPressed(42) )
				{
					vec *= 10000.0;
				}
				else
				{
					vec.x = 0.0;
					vec.z = 0.0;
					vec.y = -300.0;
				}

				zINFO("B: (Spacer) ApplyForceCM on Vob " + vec.x + "/" + vec.y + "/" + vec.z);
	
				vob->GetRigidBody()->ApplyImpulseCM(vec);

				zCTimer timer;
				timer.ResetTimer();

				time_t time;
				std::time(&time);

				int i = 0;

				zVEC3 prev = pos;
				zVEC3 cur = pos;
				int equalCtr = 0;

				while ( 1 )
				{
					prev = cur;

					do {
						OnDraw(dc);
					} while(!__drawFinished);

					++i;

					cur = vob->GetTranslation();

					if ( prev == cur )
						++equalCtr;

					zINFO(6,"B: APPLYPHYSICS: Position " + cur.x + "," + cur.y "," + cur.z ); // 3129,
	
					time_t time1;
					std::time(&time1);

					auto diff = difftime(time1, time);
					if ( equalCtr >= 3 )
						break;
					if ( diff >= 7 )
					{
						if ( timer.frameTime < 7 )
						{
							equalCtr = 0;
							continue;
						}

						auto result = CWnd::MessageBox("Time elapsed\r\nContinue?", "Apply Physics", 4u);
						if ( result != IDNO )
						{
							std::time(&Time);
							equalCtr = 0;
							continue;
						}

						if ( CWnd::MessageBox("Reset position of vob?", "Apply Physics", 4u) == IDYES )
						{
							auto cdyn = vob->collDetectionDynamic;
							auto cstat = vob->collDetectionStatic
							vob->SetSleeping(0);
							vob->SetPhysicsEnabled(0);
							vob->SetCollDetDyn(0);
							vob->SetCollDetStat(0);
							vob->SetPositionWorld(pos);
							vob->SetCollDetDyn(cdyn);
							vob->SetCollDetStat(cstat);
							prev = pos;
						}
						
						// makes no sense, but I don't really want to put effort into
						// understanding and restructuring this code
						if ( result != IDNO )
						{
							equalCtr = 0;
							continue;
						}

						break;
					}
				}

				zINFO("B: SPC: calculated physics for " + i + " frames.");
	
				vob->SetPhysicsEnabled(physicsEnabled);
				vob->SetPositionWorld(cur);
				zCView::MFC_KeyUp(0x1Du);
			}
		}
	}
}
