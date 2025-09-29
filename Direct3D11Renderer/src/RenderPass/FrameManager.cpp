#include "RenderPass/FrameManager.h"
#include "Bindable/BindableCommon.h"

void FrameManager::Accept(Job job, size_t target) noexcept
{
	passes[target].Accept(std::move(job));
}

void FrameManager::Excecute(Graphics& gfx) const
{
	// normally this would be a loop with each pass defining it setup / etc.
	// and later on it would be a complex graph with parallel execution contingent
    // on input / output requirements

	// Main phong lighting pass
	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	passes[0].Excecute(gfx);
	
	// Outline mask pass
	Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
	NullPixelShader::Resolve(gfx)->Bind(gfx);
	passes[1].Excecute(gfx);

	// Outline draw pass
	Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
	struct SolidColorBuffer
	{
		DirectX::XMFLOAT4 color = { 1.0f, 0.4f, 0.4f, 1.0f };
	} solidColorBuffer;

	PixelConstantBuffer<SolidColorBuffer>::Resolve(gfx, solidColorBuffer, 1u)->Bind(gfx);
	passes[2].Excecute(gfx);
}

void FrameManager::Reset() noexcept
{
	for (auto& pass : passes)
	{
		pass.Reset();
	}
}