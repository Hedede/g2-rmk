// _roman/oNpc_Damage.cpp
void oCNpc::oSDamageDescriptor::SetFXHit(oCVisualFX* fx)
{
	if (fx != fxHit) {
		Release(fxHit);
		fxHit = fx;
		AddRef(fx);
	}
}

void oCNpc::oSDamageDescriptor::SetVisualFX(oCVisualFX* fx)
{
	if (fx != visualFx) {
		Release(visualFx);
		visualFx = fx;
		AddRef(fx);
	}
}

void oCNpc::oSDamageDescriptor::Release()
{
	Release(visualFx);
	Release(fxHit);
}

int oCNpc::GetBluntDamage(oCItem* item)
{
	if (item) {
		int damage = item->GetDamageByIndex(DAM_INDEX_BLUNT);
		return damage - protection[1];
	}

	return 0;
}

int oCNpc::GetWeaponDamage(oCItem* item)
{
	int result = 0;
	if ( item ) {
		for (unsigned i = 0; i < DAM_INDEX_MAX; ++i) {
			int dam = item->GetDamageByIndex(i) - protection[i];
			if (dam > 0)
				result += dam;
		}
	}
	return result;
}

