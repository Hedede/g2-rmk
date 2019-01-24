struct zCUnderwaterPFX : zCParticleFX {
	void ~zCUnderwaterPFX() override = default;
	void Render(zTRenderContext& ctx) override
	{
		ProcessParticles();
		zCParticleFX::Render(ctx);
	}

private:
	zVEC3 __camPos;
};
