#include "Renderable/TestCube.h"
#include "Geometry/Cube.h"
#include "Bindable/BindableCommon.h"
#include "Bindable/Stencil.h"
#include "imgui.h"

TestCube::TestCube(Graphics& gfx, float size)
{
	namespace dx = DirectX;

	// Create cube geometry using the Cube class
	auto model = Cube::Make();
	model.Transform(dx::XMMatrixScaling(size, size, size));

	const auto geometryTag = "$cube." + std::to_string(size);

	// Main cube rendering bindables
	AddBindable(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBindable(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	AddBindable(Texture::Resolve(gfx, "assets\\models\\brick_wall\\brick_wall_diffuse.jpg"));
	AddBindable(Sampler::Resolve(gfx));

	auto pvs = VertexShader::Resolve(gfx, "shaders\\Output\\BlinnPhong_Diffuse_VS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBindable(std::move(pvs));

	AddBindable(PixelShader::Resolve(gfx, "shaders\\Output\\BlinnPhong_Diffuse_PS.cso"));

	AddBindable(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));

	AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBindable(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBindable(std::make_shared<TransformConstantBuffer>(gfx, *this, TransformConstantBuffer::ShaderStage::Both, 0u, 2u));

	// Add stencil write for main object
	AddBindable(Stencil::Resolve(gfx, Stencil::Mode::Write));

	// Setup outline effect bindables with solid cube (Position + Normal only)
	auto solidModel = Cube::MakeSolid();
	solidModel.Transform(dx::XMMatrixScaling(size, size, size));
	const auto solidGeometryTag = "$cube_solid." + std::to_string(size);

	outlineEffect.push_back(VertexBuffer::Resolve(gfx, solidGeometryTag, solidModel.vertices));
	outlineEffect.push_back(IndexBuffer::Resolve(gfx, solidGeometryTag, solidModel.indices));

	auto solidVs = VertexShader::Resolve(gfx, "shaders\\Output\\SolidColor_VS.cso");
	auto solidVsbc = solidVs->GetByteCode();
	outlineEffect.push_back(std::move(solidVs));
	outlineEffect.push_back(PixelShader::Resolve(gfx, "shaders\\Output\\SolidColor_PS.cso"));

	// Simple solid color material for outline
	struct SolidColorMaterial
	{
		DirectX::XMFLOAT4 solidColor = { 1.0f, 0.4f, 0.4f, 1.0f }; // Red-orange outline - 16 bytes
	} solidMaterial;
	static_assert(sizeof(SolidColorMaterial) % 16 == 0, "Solid color material constant buffer must be multiple of 16 bytes");
	outlineEffect.push_back(PixelConstantBuffer<SolidColorMaterial>::Resolve(gfx, solidMaterial, 1u));

	outlineEffect.push_back(InputLayout::Resolve(gfx, solidModel.vertices.GetLayout(), solidVsbc));
	outlineEffect.push_back(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	outlineEffect.push_back(std::make_shared<TransformConstantBuffer>(gfx, *this, TransformConstantBuffer::ShaderStage::Both, 0u, 2u));

	// Add stencil mask for outline (only render where stencil buffer is not equal to 1)
	outlineEffect.push_back(Stencil::Resolve(gfx, Stencil::Mode::Mask));
}

void TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void TestCube::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	if (renderingOutline)
	{
		// Scale up slightly for outline effect, then apply rotation and translation
		return DirectX::XMMatrixScaling(1.03f, 1.03f, 1.03f) *
			DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	}
	else
	{
		return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	}
}

void TestCube::RenderOutline(Graphics& gfx) const noexcept(!_DEBUG)
{
	// Set flag to use outline transform
	renderingOutline = true;

	// Bind all outline effect bindables and render
	for (auto& bindable : outlineEffect)
	{
		bindable->Bind(gfx);
	}
	gfx.DrawIndexed(static_cast<UINT>(const_cast<TestCube*>(this)->QueryBindable<IndexBuffer>()->GetCount()));

	// Reset flag
	renderingOutline = false;
}

void TestCube::RenderWithOutline(Graphics& gfx) const noexcept(!_DEBUG)
{
	// First render the main object (this writes to the stencil buffer)
	Render(gfx);

	// Then render the outline (this uses the stencil mask)
	RenderOutline(gfx);
}

void TestCube::SpawnControlWindow(Graphics& gfx, const char* name) noexcept
{
	if (ImGui::Begin(name))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		ImGui::Text("Shading");
		bool changed0 = ImGui::SliderFloat("Spec. Reflectance", &pmc.specularReflectance, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Shininess", &pmc.specularShininess, 0.0f, 100.0f);
		if (changed0 || changed1)
		{
			QueryBindable<PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
	}
	ImGui::End();
}