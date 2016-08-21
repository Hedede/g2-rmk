void Game_DeleteAllPfx(zCTree<zCVob>* vobtree)
{
	for (auto node = vobtree; node; node = node->next) {
		zCVob vob* = node->data();
		auto* pfx = dynamic_cast<zCParticleFX*>(vob->visual);
		if (pfx) {
			vob->Release();
		}
		auto child = node->firstChild;
		Game_DeleteAllPfx(child);
	}
}
