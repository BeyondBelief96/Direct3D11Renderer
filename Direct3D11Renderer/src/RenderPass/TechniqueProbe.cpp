#include "RenderPass/TechniqueProbe.h"

void TechniqueProbe::SetTechnique(Technique* pTech) noexcept
{
	this->pTechnique = pTech;
	OnSetTechnique();
}

void TechniqueProbe::SetStep(Step* pStep)
{
	this->pStep = pStep;
	OnSetStep();
}