#include "Effect.h"
#include "GraphicsDevice.h"
#include "IEffectPimpl.h"
#include "../Content/XnbReader.h"

namespace Nxna
{
namespace Graphics
{
	Effect::Effect(GraphicsDevice* device, const byte* effectCode, int effectCodeLength)
	{
		m_device = device;
		m_currentTechniqueIndex = -1;
		m_pimpl = device->CreateEffectPimpl(this);
	}

	Effect::Effect(GraphicsDevice* device)
	{
		m_device = device;
		m_currentTechniqueIndex = -1;
		m_pimpl = device->CreateEffectPimpl(this);
	}

	Effect::Effect(GraphicsDevice* device, Pvt::IEffectPimpl* pimpl)
	{
		m_device = device;
		m_currentTechniqueIndex = -1;
		m_pimpl = pimpl;
	}

	Effect::~Effect()
	{
		for (auto itr = m_techniques.begin(); itr != m_techniques.end(); itr++)
		{
			EffectTechnique* technique = (*itr);
			delete technique;
		}

		for (auto itr = m_hiddenTechniques.begin(); itr != m_hiddenTechniques.end(); itr++)
		{
			EffectTechnique* technique = (*itr);
			delete technique;
		}
	}

	EffectParameter* Effect::GetParameter(int index)
	{
		return m_pimpl->GetParameter(index);
	}

	EffectParameter* Effect::GetParameter(const char* name)
	{
		return m_pimpl->GetParameter(name);
	}

	int Effect::GetNumParameters()
	{
		return m_pimpl->GetNumParameters();
	}

	EffectTechnique* Effect::GetTechnique(int index)
	{
		if (index < 0 || index >= m_techniques.size())
			throw ArgumentException("index");

		return m_techniques[index];
	}

	EffectTechnique* Effect::GetTechnique(const char* name)
	{
		for (auto itr = m_techniques.begin(); itr != m_techniques.end(); itr++)
		{
			if ((*itr)->Name == name)
				return (*itr);
		}

		// apparently no technique by that name exists
		throw ArgumentException(name);
	}

	int Effect::GetNumTechniques()
	{
		return m_techniques.size();
	}

	void Effect::SetCurrentTechnique(EffectTechnique* technique)
	{
		if (technique->m_parentEffect != this || technique->m_hidden == true)
			throw InvalidOperationException("The technique does not belong to this effect.");

		m_currentTechniqueIndex = technique->m_index;
	}

	EffectTechnique* Effect::GetCurrentTechnique()
	{
		if (m_currentTechniqueIndex < 0)
			return nullptr;

		return m_techniques[m_currentTechniqueIndex];
	}

	void Effect::OnApply()
	{
		if (m_currentTechniqueIndex < 0)
			throw InvalidOperationException("The effect has no current technique set");

		m_pimpl->Apply(m_currentTechniqueIndex);
	}

	EffectTechnique* Effect::CreateTechnique(const char* name, bool hidden)
	{
		EffectTechnique* technique;

		if (hidden)
		{
			technique = new EffectTechnique(this, name, m_hiddenTechniques.size(), true);
			m_hiddenTechniques.push_back(technique);
		}
		else
		{
			technique = new EffectTechnique(this, name, m_techniques.size(), false);
			m_techniques.push_back(technique);

			if (m_techniques.size() == 1)
				m_currentTechniqueIndex = 0;
		}

		return technique;
	}

	void EffectTechnique::Apply()
	{
		if (m_parentEffect->GetCurrentTechnique() != this)
			throw InvalidOperationException("The technique you are trying to Apply() is not the current technique.");

		m_parentEffect->OnApply();
	}
}
}