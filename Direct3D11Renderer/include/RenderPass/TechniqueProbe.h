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
	bool VisitBuffer(D3::ConstantBufferData& bufferData);
protected:
	virtual void OnSetTechnique() {};
	virtual void OnSetStep() {};
	virtual bool OnVisitBuffer(D3::ConstantBufferData& bufferData) = 0;
	Technique* pTechnique = nullptr;
	Step* pStep = nullptr;
	size_t techniqueIdx = std::numeric_limits<size_t>::max();
	size_t stepIdx = std::numeric_limits<size_t>::max();
	size_t bufferIdx = std::numeric_limits<size_t>::max();
};
