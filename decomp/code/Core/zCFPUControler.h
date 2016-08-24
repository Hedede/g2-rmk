struct zCFPUControler {
	zCFPUControler()
	{
		ComputeDefaultControlWord();
	}

	unsigned saved_control_word;
	unsigned default_control_word;

private:
	unsigned ComputeDefaultControlWord();
};

unsigned zCFPUControler::GetCurrentControlWord()
{
	return _controlfp(0, 0);
}

void zCFPUControler::SaveCurrentControlWord()
{
	saved_control_word = GetCurrentControlWord();
}

void zCFPUControler::RestoreSavedControlWord()
{
	_controlfp(saved_control_word, 0xFFFFFFFF);
}

void zCFPUControler::RestoreDefaultControlWord()
{
	_controlfp(default_control_word, 0xFFFFFFFF);
}

void zCFPUControler::SetControlWord(unsigned newValue) // __stdcall?
{
	_controlfp(newValue, 0xFFFFFFFF);
}

void zCFPUControler::SetPrecision_24()
{
	_controlfp(default_control_word & 0xFFFEFFFF | 0x20000, 0x30000u);
}

void zCFPUControler::SetPrecision_53()
{
	_controlfp(default_control_word & 0xFFFDFFFF | 0x10000, 0x30000u);
}

void zCFPUControler::SetPrecision_64()
{
	_controlfp(default_control_word & 0xFFFCFFFF, 0x30000u);
}

unsigned zCFPUControler::ComputeDefaultControlWord()
{
	_controlfp(0x9001Fu, 0xFFFFFFFF);
	default_control_word = GetCurrentControlWord() & 0xFFF4FFE0 ^ 0xA001F;;
	RestoreDefaultControlWord();
}

// _dieter\\zTools.cpp
void zCFPUControler::PrintStatus()
{
	zerr.Separator("");
	auto cw = GetCurrentControlWord();

	zINFO_B(1"D: FPU: Checking FPU Control Word: controlfp() returns \""_s + cw + "\""); // 661

	zSTRING desc;
	if (cw & 0x1000000) {
		desc += "_DN_FLUSH";
	} else {
		desc += "_DN_SAVE";
	}

	zINFO(1, "D: FPU: ... Denormal control (_MCW_DM_): " + desc); // 666
	zINFO(1, "D: FPU: ... Interrupt exception (_MCW_EM)"); // 670
	int none = 1;
	if ( !(cw & 0x10) ) {
		zINFO(1, "D: FPU: ...... _EM_INVALID"); // 671
		none = 0;
	}
	if ( !(cw & 0x80000) ) {
		zINFO(1,"D: FPU: ...... _EM_DENORMAL"); // 672
		none = 0;
	}
	if ( !(cw & 8) ) {
		zINFO(1,"D: FPU: ...... _EM_ZERODIVIDE"); // 673,
		none = 0;
	}
	if ( !(cw & 4) ) {
		zINFO(1,"D: FPU: ...... _EM_OVERFLOW"); // 674
		none = 0;
	}
	if ( !(cw & 2) ) {
		zINFO(1,"D: FPU: ...... _EM_UNDERFLOW"); // 675
		none = 0;
	}
	if ( !(cw & 1) ) {
		zINFO(1,"D: FPU: ...... _EM_INEXACT"); // 676,
	} else if ( none ) {
		zINFO(1, "D: FPU: ...... none"); // 677
	}

	if ( cw & 0x40000 ) {
		desc = "_IC_AFFINE";
	} else {
		desc = "_IC_PROJECTIVE";
	}

	zINFO(1,"D: FPU: ... Infinity control (_MCW_IC): " + desc); // 682


	if ( (cw << 8) & 3 ) {
		desc = "_RC_CHOP";
	} else if ( (cw << 8) & 2 ) {
		desc = "_RC_UP";
	} else if ( (cw << 8) & 1 ) {
		desc = "_RC_DOWN";
	} else {
		desc = "_RC_NEAR";
	}

	zINFO(1, "D: FPU: ... Rounding control (_MCW_RC): " + desc); // 689

	if ( cw & 0x20000) {
		desc =  "_PC_24";
	} else if ( cw & 0x10000 ) {
			desc = "_PC_53";
	} else {
		desc = "_PC_64";
	}

	zINFO(1,"D: FPU: ... Precision control (_MCW_PC): " + desc); // 695
	zINFO_C(""); // 697,
}
