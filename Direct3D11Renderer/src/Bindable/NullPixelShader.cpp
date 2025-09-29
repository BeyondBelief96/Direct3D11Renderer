#include "Bindable/NullPixelShader.h"
#include "Exceptions/GraphicsExceptions.h"
#include "Bindable/BindableCache.h"

NullPixelShader::NullPixelShader(Graphics& gfx)
{

}

void NullPixelShader::Bind(Graphics& gfx) noexcept
{
	// no shader bound
	GetContext(gfx)->PSSetShader(nullptr, nullptr, 0u);
}

std::shared_ptr<NullPixelShader> NullPixelShader::Resolve(Graphics& gfx)
{
	return BindableCache::Resolve<NullPixelShader>(gfx);
}

std::string NullPixelShader::GenerateUID()
{
	return typeid(NullPixelShader).name();
}

std::string NullPixelShader::GetUID() const noexcept
{
	return GenerateUID();
}