#include "Renderable/TestCube.h"
#include "Geometry/Cube.h"
#include "DynamicConstantBuffer/DynamicConstantBuffer.h"
#include "Bindable/DynamicConstantBufferBindable.h"
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

	Technique shade("Shade");
	{
		Step only(0);
		only.AddBindable(Texture::Resolve(gfx, "assets\\models\\brick_wall\\brick_wall_diffuse.jpg", 0u));
		only.AddBindable(Sampler::Resolve(gfx));
		auto pvs = VertexShader::Resolve(gfx, "shaders\\Output\\BlinnPhong_Diffuse_VS.cso");
		auto pvsbc = pvs->GetByteCode();
		only.AddBindable(std::move(pvs));
		only.AddBindable(PixelShader::Resolve(gfx, "shaders\\Output\\BlinnPhong_Diffuse_PS.cso"));
		D3::LayoutBuilder layout;
		layout.Add<D3::ElementType::Float>("specularIntensity");
		layout.Add<D3::ElementType::Float>("specularPower");
		auto buffer = D3::ConstantBufferData(std::move(layout));
		buffer["specularIntensity"] = 0.6f; 
		buffer["specularPower"] = 30.0f; 
		only.AddBindable(std::make_shared<CachingDynamicPixelConstantBufferBindable>(gfx, buffer, 1u));
		only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
		only.AddBindable(std::make_shared<TransformConstantBuffer>(gfx));
		shade.AddStep(std::move(only));
	}
	AddTechnique(std::move(shade));

	Technique outline("Outline");
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

		D3::LayoutBuilder layout;
		layout.Add<D3::ElementType::Float4>("color");
		auto buffer = D3::ConstantBufferData(std::move(layout));
		buffer["color"] = DirectX::XMFLOAT4{ 1.0f, 0.4f, 1.0f, 1.0f };
		draw.AddBindable(std::make_shared<CachingDynamicPixelConstantBufferBindable>(gfx, buffer, 1u));
		draw.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
		class TransformConstantBufferScaling : public TransformConstantBuffer
		{
		public:
			TransformConstantBufferScaling(Graphics& gfx, float scale = 1.04f)
				: TransformConstantBuffer(gfx), buffer(MakeLayout()) 
			{
				buffer["scale"] = scale;
			}

			void Accept(TechniqueProbe& probe) override
			{
				probe.VisitBuffer(buffer);
			}

			void Bind(Graphics& gfx) noexcept override
			{
				const auto scale = buffer["scale"];
				const auto scaleMatrix = DirectX::XMMatrixScaling(scale, scale, scale);
				auto xf = GetTransformBuffer(gfx);
				xf.modelView = xf.modelView * scaleMatrix;
				xf.modelViewProj = xf.modelViewProj * scaleMatrix;
				UpdateBindImpl(gfx, xf);
			}

		private:
			static D3::LayoutBuilder MakeLayout()
			{
				D3::LayoutBuilder layout;
				layout.Add<D3::ElementType::Float>("scale");
				return layout;
			}
			D3::ConstantBufferData buffer;
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

		class Probe : public TechniqueProbe
		{
		public:
			void OnSetTechnique() override
			{
				ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, pTechnique->GetName().c_str());
				bool active = pTechnique->IsActive();
				ImGui::Checkbox((std::string("Technique Active") + pTechnique->GetName()).c_str(), &active);
				pTechnique->SetActiveState(active);
			}

			bool VisitBuffer(D3::ConstantBufferData& buffer) override
			{
				bool dirty = false;
				const auto dCheck = [&dirty](bool changed) {dirty = dirty || changed; };

				if (auto v = buffer["scale"]; v.Exists())
				{
					float scaleValue = v;  
					dCheck(ImGui::SliderFloat("Scale", &scaleValue, 1.0f, 2.0f, "%.3f"));
					v = scaleValue; 
				}
				if(auto v = buffer["color"]; v.Exists())
				{
					DirectX::XMFLOAT4 colorValue = v; 
					dCheck(ImGui::ColorPicker4("Color", reinterpret_cast<float*>(&colorValue)));
					v = colorValue; 
				}
				if(auto v = buffer["specularIntensity"]; v.Exists())
				{
					float specularIntensityValue = v; 
					dCheck(ImGui::SliderFloat("Specular Intensity", &specularIntensityValue, 0.0f, 1.0f));
					v = specularIntensityValue; 
				}
				if(auto v = buffer["specularPower"]; v.Exists())
				{
					float specularPowerValue = v; 
					dCheck(ImGui::SliderFloat("Glossiness", &specularPowerValue, 1.0f, 100.0f, "%.1f", 1.5f));
					v = specularPowerValue; 
				}

				return dirty;
			}
		};

		static Probe probe;
		Accept(probe);
	}
	ImGui::End();
}