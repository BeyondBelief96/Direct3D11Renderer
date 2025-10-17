#include "RenderPass/TechniqueProbe.h"

void TechniqueProbe::SetTechnique(Technique* pTech) noexcept
{
	this->pTechnique = pTech;
	techniqueIdx++;
	OnSetTechnique();
}

void TechniqueProbe::SetStep(Step* pStep)
{
	this->pStep = pStep;
	stepIdx++;
	OnSetStep();
}

bool TechniqueProbe::VisitBuffer(D3::ConstantBufferData& bufferData)
{
	bufferIdx++;
	return OnVisitBuffer(bufferData);
}