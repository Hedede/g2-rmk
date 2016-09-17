// _bert/zBinkPlayer.cpp
struct zCBinkPlayer : zCVideoPlayer {
	zCBinkPlayer() : zCVideoPlayer()
	{
		zINFO(3,"B: VP: Initialize zBinkPlayer"); // 45, 
	}
	~zCBinkPlayer() override
	{
		if (binkHandle)
			CloseVideo();
		zINFO(3,"B: VP: zBinkPlayer deinitialized"); // 62
	}

	static int GetPixelFormat(zTRndSurfaceDesc& surf)
	{
		int type = -1;
		int bits = surf.pf_rgb_bit_count;
		switch ( bits ) {
		case 16:
			if ( (surf.pf_g_bit_mask & 0x400) == 0x400 ) {
				type = BINKSURFACE565;
			} else if ( !(surf.pf_g_bit_mask & 0x400) ) {
				type = BINKSURFACE555;
			}
			break;
		case 24:
			if ( (surf.pf_r_bit_mask & 1) == 1 ) {
				type = BINKSURFACE24R;
			} else if ( (surf.pf_b_bit_mask & 1) == 1 ) {
				type = BINKSURFACE24;
			}
			break;
		case 32:
			if ( (surf.pf_r_bit_mask & 1) == 1 ) {
				type = BINKSURFACE32R;
			} else if ( (surf.pf_b_bit_mask & 1) == 1 ) {
				type = BINKSURFACE32;
			}
			break;
		default:
			zWARNING("B: VP: Unknown rgb-count!"); // 388,
			break;
		}
		return type;
	} 

	int OpenVideo(zSTRING fileName) override;
	int CloseVideo() override
	{
		if ( binkHandle ) {
			zINFO_B(4, "B: VP: Closing Videofile: \"" + GetVideoFilename() +  "\""); // 128
			BinkClose(binkHandle);
			binkHandle = 0;
			int ret = zCVideoPlayer::CloseVideo(&this->vt);
			zINFO_C(4, "B: VP: Closing Videofile finished"); // 134
			return ret;
		}
		return 0;
	}

	int PlayInit(int frameNr) override;
	int PlayFrame() override;
	int PlayDeinit() override;
	int Pause() override
	{
		BinkPause(binkHandle, 1);
		return zCVideoPlayer::Pause();
	}
	int Unpause() override
	{
		BinkPause(binkHandle, 0);
		return zCVideoPlayer::Unpause();
	}
	int IsPlaying() override
	{
		return zCVideoPlayer::IsPlaying && binkHandle &&
		       (unk1 || bink->Frames > bink->FrameNum);
	}
	int ToggleSound() override
	{
		if ( zCVideoPlayer::ToggleSound() ) {
			SetSoundVolume(1.0);
		} else {
			SetSoundVolume(0.0);
		}
		return soundEnabled;
	}
	void SetSoundVolume(float vol) override
	{
		BinkSetVolume(binkHandle, 0, (vol * 65536.0));
	}

	int PlayGotoNextFrame() override
	{
		BinkNextFrame(binkHandle);
		return 1;
	}
	void PlayWaitNextFrame() override
	{
		while ( IsPlaying() ) {
			if ( !BinkWait(binkHandle) )
				break;
			PlayHandleEvents();
		}
		return 1;
	}
	bool PlayHandleEvents() override
	{
		return handleEvents == 0;
	}

	void SetFullscreen(int full, zSTRING texture)
	{
		fullScreen = full;
		fsTexture = texture;
	}

private:
	BINK* binkHandle = 0;
	int pixelFormat = -1;
	short sizex;
	short sizey;
	int unkz3 = 0;
	int handleEvents = 1;
	int fullScreen = 1;
	zSTRING fsTexture;
	zCView* view;
	RECT srcRect;
	RECT dstRect;
};

int zCBinkPlayer::OpenVideo(zSTRING fileName)
{

	zFILE_FILE file(fileName);
	if ( file.GetExt().IsEmpty() )
		file.SetExt("BIK");
	if ( !file.Exists() ) {
		zWARNING("B: VP: File \"" + file.GetFile() + "\" does not exist."); // 74
		return 0;
	}

	zINFO_B(4, "B: VP: Open Videofile: \"" + file.GetFile() "\""); // 78

	if (auto mss = dynamic_cast<zCSndSys_MSS*>(zsound)) {
		if ( auto drv = mss->GetDigitalDriverHandle() ) {
			BinkSetSoundSystem(BinkOpenMiles, drv);
			BinkSetSoundOnOff(binkHandle, 1);
			BinkSetVolume(binkHandle, 0, 0x10000);
		}
	}

	fileName = file.GetFullPath();
	binkHandle = BinkOpen(fileName.Data(), 0);
	if ( binkHandle ) {
		zCVideoPlayer::OpenVideo(fileName);
		zINFO(4, "B: VP: Filesize: "_s + binkHandle->Size / 1024.0 + " kb"); //109,
		zINFO(4, "B: VP: Resolution: "_s + binkHandle->decompheight + "x" + binkHandle->decompwidth); //110,
		zINFO(4, "B: VP: Framecount: "_s + binkHandle->Frames); // 111
		zINFO(4, "B: VP: Framerate: "_s + binkHandle->fileframerate + " fps"); // 112,
		zINFO(4, "B: VP: Trackcount (Sound): " + binkHandle->NumTracks); // 113
	} else {
		zWARNING( "B: VP: Could not open videofile \"" + fileName + "\""); // 117
	}

	zINFO_C("B: VP: Open Videofile finished"); // 119
	return binkHandle != 0;
}

int zCBinkPlayer::PlayInit(int frameNr)
{
	if ( !binkHandle )
		return 0;
	zINFO_B(3,"B: VP: Initialize Videoplay: \"" + GetVideoFilename() "\""); // 143,
	if ( !zCVideoPlayer::PlayInit(frameNr) ) {
		CloseVideo();
		return 0;
	}

	if ( frameNr > 0 )
		BinkGoto(binkHandle, frameNr, 0);

	zTRndSurfaceDesc surf;
	pixelFormat = -1;
	sizex = -1;
	sizey = -1;

	int doPlay = 0;

	if ( zrenderer->Vid_Lock(surf) ) {
		zrenderer->Vid_Unlock();
		pixelFormat = GetPixelFormat(surf);
		if ( pixelFormat >= 0 ) {
			zINFO(4,"B: VP: Pixelformat: "_s + pixelFormat); //174,
		} else {
			zINFO(4,"B: VP: Unknown pixelformat."); // 173
		}

		if ( pixelFormat >= 0 ) {
			doPlay = 1;
			if ( binkHandle->Height > surf.height ||
			     binkHandle->Width > surf.width ) {

				zWARNING("B: VP: Video won't play because of low resolution: "_s + surf.width + "x" + surf.height); // 190
				doPlay = 0;
			}
		}
	} else {
		zFAULT("B: VP: Renderer is locked."); // 165
	}

	auto scaleVideos = zoptions->ReadBool(zOPT_SEC_GAME, "scaleVideos", 1);
	if ( doPlay ) {
		float hratio = surf.height / binkHandle->Height;
		float wratio = surf.width / binkHandle->Width;
		float scale = 0.0;
		if ( wratio >= hratio )
			scale = hratio;
		else
			scale = wratio;

		if (!scaleVideos)
			scale = 1.0;

		sizex = (surf.height - binkHandle->Height * scale) * 0.5;
		sizey = (surf.width  - binkHandle->Width * scale)  * 0.5;
		if ( sizex < 0 || sizey < 0 ) {
			doPlay = false;
		} else {
			srcRect.left = 0;
			srcRect.top = 0;
			srcRect.right = binkHandle->Width + 2 * sizex;
			srcRect.bottom = binkHandle->Height + 2 * sizey;
			destRect.left = 0;
			destRect.top = 0;
			destRect.right = surf.width;
			destRect.bottom = surf.height;
			zINFO(4,"B: VP: Scale-Factor: "_s + scale); // 225
			zINFO(4,"B: VP: Video-Offset: "_s + sizex + "/" + sizey); // 226
			if ( fullScreen ) {
				fsTexture = "default.tga";
				Delete(view);
				view = new zCView(0, 0, 0x2000, 0x2000, 2);
				view->SetColor(GFX_BLACK);
				view->InsertBack(fsTexture);
				view->InsertItem(screen, view, 0);
				view->Render(screen);
				zrenderer->Vid_Blit(1, srcRect, dstRect);
				view->Render(screen);
				zrenderer->Vid_Blit(1, srcRect, dstRect);
			}
		}
	}

	byte_8923CC = 1;

	if ( !doPlay )
		CloseVideo();
	zINFO_C(3,"B: VP: Initialize Videoplay finished"); // 256
	return doPlay;
}

int zCBinkPlayer::PlayDeinit()
{
	zINFO_B(3,"B: VP: Deinitialize Videoplay: \"" + GetVideoFilename() +  "\""); // 262
	if ( binkHandle ) {
		zINFO(4, "B: VP: Frames played: "_s + binkHandle->playedframes); // 266
		zINFO(4, "B: VP: Skipped blits: "_s + binkHandle->skippedblits); // 267
		zINFO(4, "B: VP: Memory used: "_s + binkHandle->totalmem / 1024.0 + " kb"); // 271
		zINFO(4, "B: VP: Time open: "_s + binkHandle->timeopen  + " sec"); // 272
		zINFO(4, "B: VP: Time blit: "_s + binkHandle->timeblit  + " sec"); // 273
	}
	int ret = zCVideoPlayer::PlayDeinit();
	Delete(view);

	zINFO_C(3, "B: VP: Deinitialize Videoplay finished"); // 280
	return ret;
}

int zBinkPlayer::PlayFrame()
{
	if ( IsPlaying() ) {
		zCVideoPlayer::PlayFrame();
		if ( IsPlaying() ) {
			BinkDoFrame(binkHandle);
			PlayGotoNextFrame();
			PlayWaitNextFrame();
			zTRndSurfaceDesc surf;
			if ( zrenderer->Vid_Lock(surf) ) {
				if ( pixelFormat < 0 ) {
					zWARNING("B: VP: Unknown pixel-format"); // 322
				} else {
					BinkCopyToBuffer(binkHandle, surf.surface, surf.surfPitch, surf.height, sizex, sizey, pixelFormat);
				}
				zrenderer->Vid_Unlock();
				if ( unk ) {
					zrenderer->Vid_Blit(0, &srcRect, &dstRect);
					byte_8923CC = 1;
				}
			}
		}
		return 1;
	}
	return 0;
}
