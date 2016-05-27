using LerpFunc = float(float val)

LerpFunc* GetLerpFunc(int typ1, int typ2)
{
	switch ( typ1 ) {
	case 4:
		switch ( typ2 ) {
		default:
			return LerpSlowSlow;
		case 3:
			return LerpStep;
		case 1:
		case 2:
			return LerpSlowLinear;
		case 5:
			return LerpSlowFast;
		}
	case 3:
		switch ( typ2 ) {
		default:
			return LerpSlowSlow;
		case 3:
			return LerpStep;
		case 1:
		case 2:
			return LerpSlowLinear;
		case 5:
			return LerpSlowFast;
		}
	case 2:
		switch ( typ2 ) {
		case 3:
			return LerpStep;
		case 5:
			return LerpLinearFast;
		case 4:
			return LerpLinearSlow;
		default:
			return LerpLinear;
		}
	case 1:
		switch ( typ2 ) {
		case 3:
			return LerpStep;
		case 5:
			return LerpLinearFast;
		case 4:
			return LerpLinearSlow;
		default:
			return LerpLinear;
		}
	case 5:
		switch ( typ2 ) {
		case 3:
			return LerpStep;
		case 4:
			return LerpFastSlow;
		case 1:
		case 2:
			return LerpFastLinear;
		case 5:
			return LerpFastFast;
		default:
			return LerpLinear;
		}

	case 6:
		switch (typ2) {
		case 3:
			return LerpStep;
		case 4:
			return LerpLinearSlow;
		default:
			return LerpLinear;
		}

	default:
		return LerpLinear;
	}
}
