#pragma once
#include "Bindable.h"
#include <vector>
#include <wrl.h>
#include <memory>

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
	IndexBuffer(Graphics& gfx, std::string tag, const std::vector<unsigned short>& indices);
	void Bind(Graphics& gfx) noexcept override;
	UINT GetCount() const noexcept;
	std::string GetUID() const noexcept override;

	static std::shared_ptr<IndexBuffer> Resolve(Graphics& gfx, const std::string tag,
		const std::vector<unsigned short>& indices);
	template<typename... Ignore>
	static std::string GenerateUID(const std::string& tag, Ignore&&... ignore)
	{
		return GenerateUID_(tag);
	}
protected:
	UINT count;
	std::string tag;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
private:
	static std::string GenerateUID_(const std::string& tag);
};