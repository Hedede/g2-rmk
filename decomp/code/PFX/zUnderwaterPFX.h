struct zCUnderwaterPFX : zCParticleFX {
	void ~zCUnderwaterPFX() override = default;
	void Render(zTRenderContext&) override;

private:
	int unk[3];
};
