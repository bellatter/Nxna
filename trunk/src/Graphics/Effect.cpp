#include "Effect.h"
#include "GraphicsDevice.h"
#include "IEffectPimpl.h"
#include "../Content/XnbReader.h"

namespace Nxna
{
namespace Graphics
{
	Effect::Effect(GraphicsDevice* device)
	{
		m_device = device;
		m_pimpl = device->CreateEffectPimpl(this);
	}

	Effect::Effect(GraphicsDevice* device, Pvt::IEffectPimpl* pimpl)
	{
		m_device = device;
		m_pimpl = pimpl;
	}

	void Effect::Apply()
	{
		m_pimpl->Apply();
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
}
}