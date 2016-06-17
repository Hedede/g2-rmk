int CompareVobsByVisual(void const* a1, void const *a2)
{
	auto vob1 = *reinterpret_cast<zCVob ** const>(a1);
	auto vob2 = *reinterpret_cast<zCVob ** const>(a2);

	int key1 = 0;
	int key2 = 0;
	if (vob1->visual)
		key1 = vob1->visual->GetRenderSortKey();
	if (vob2->visual)
		key2 = vob2->visual->GetRenderSortKey();

	return key1 - key2;
}
