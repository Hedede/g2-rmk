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

	virtual void OpenVideo(zSTRING);
	virtual void CloseVideo();
	virtual void PlayInit(int);
	void PlayFrame() override
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
    result = 1;
  }
  else
  {
    result = 0;
  }
  return result;
}
		}
	}
	virtual void PlayDeinit();
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
