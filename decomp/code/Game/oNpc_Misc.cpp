oCItem* oCNpc::IdentifyMushroom(oCItem* item)
{
	RELEASE(item);

	// was turned into signed-bitmath by compiler
	auto rnd = rand()%2 == 0;

	int idx = zparser.GetIndex( rnd ? "ITFOHONEYMUSHROOM" : "ITFOSTINKMUSHROOM")
	if ( idx >= 0 )
		return ogame->GetGameWorld()->CreateVob(VOB_TYPE_ITEM,idx);
	return nullptr;
}

void oCNpc::IdentifyAllMushrooms()
{
	auto idx = zparser.GetIndex("ITFOMUSHROOM");
	if ( idx >= 0 ) {
		while ( 1 ) {
			auto item = inventory.Remove4(idx, 1);
			if ( !item )
				break;
			if ( item->HasFlag(ITEM_ACTIVE) )
				item->Equip(item);
			PutInInv(IdentifyMushroom(item));
		}
	}
}
