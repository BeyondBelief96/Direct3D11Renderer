#include "Bindable.h"
#include <array>
#include <memory>

class Rasterizer : public Bindable
{
public:
	Rasterizer(Graphics& gfx, bool twoSided = false);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Rasterizer> Resolve(Graphics& gfx, bool twoSided = false);
	static std::string GenerateUID(bool twoSided = false);
	std::string GetUID() const noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
	bool twoSided;
};