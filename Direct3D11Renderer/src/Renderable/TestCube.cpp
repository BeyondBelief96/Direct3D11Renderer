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
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Technique standard;
	{
		Step only(0);
		only.AddBindable(Texture::Resolve(gfx, "assets\\models\\brick_wall\\brick_wall_diffuse.jpg", 0u));
		only.AddBindable(Sampler::Resolve(gfx));
		auto pvs = VertexShader::Resolve(gfx, "shaders\\Output\\BlinnPhong_Diffuse_VS.cso");
		auto pvsbc = pvs->GetByteCode();
		only.AddBindable(std::move(pvs));
		only.AddBindable(PixelShader::Resolve(gfx, "shaders\\Output\\BlinnPhong_Diffuse_PS.cso"));
		only.AddBindable(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
		only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
		only.AddBindable(std::make_shared<TransformConstantBuffer>(gfx));
		standard.AddStep(only);
	}
	AddTechnique(standard);

	Technique outline;
	{
		Step mask(1);
		auto pvs = VertexShader::Resolve(gfx, "shaders\\Output\\SolidColor_VS.cso");
		auto pvsbc = pvs->GetByteCode();
		mask.AddBindable(std::move(pvs));

		mask.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
		mask.AddBindable(std::make_shared<TransformConstantBuffer>(gfx));
		outline.AddStep(mask);
	}
	{
		Step draw(2);
		auto pvs = VertexShader::Resolve(gfx, "shaders\\Output\\SolidColor_VS.cso");
		auto pvsbc = pvs->GetByteCode();
		draw.AddBindable(std::move(pvs));
		draw.AddBindable(PixelShader::Resolve(gfx, "shaders\\Output\\SolidColor_PS.cso"));
		draw.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
		class TransformConstantBufferScaling : public TransformConstantBuffer
		{
		public:
			using TransformConstantBuffer::TransformConstantBuffer;
			void Bind(Graphics& gfx) noexcept override
			{


				const auto scale = dx::XMMatrixScaling(1.04f, 1.04f, 1.04f);
				auto xf = GetTransformBuffer(gfx);
				xf.modelView = xf.modelView * scale;
				xf.modelViewProj = xf.modelViewProj * scale;
				UpdateBindImpl(gfx, xf);
			}
		};
		draw.AddBindable(std::make_shared<TransformConstantBufferScaling>(gfx));
		outline.AddStep(draw);
	}
	AddTechnique(std::move(outline));
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
		return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
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
		//ImGui::Text("Shading");
		/*bool changed0 = ImGui::SliderFloat("Spec. Reflectance", &pmc.specularReflectance, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Shininess", &pmc.specularShininess, 0.0f, 100.0f);
		if (changed0 || changed1)
		{
			QueryBindable<PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}*/
	}
	ImGui::End();
}