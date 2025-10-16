#pragma once

#include "Step.h"

namespace D3 
{
	class ConstantBufferData;
}

class Technique;
class Step;

class TechniqueProbe
{
public:
	void SetTechnique(Technique* pTech) noexcept;
	void SetStep(Step* pStep);
	virtual bool VisitBuffer(D3::ConstantBufferData& bufferData) = 0;
protected:
	virtual void OnSetTechnique() {};
	virtual void OnSetStep() {};
	Technique* pTechnique = nullptr;
	Step* pStep = nullptr;
};
