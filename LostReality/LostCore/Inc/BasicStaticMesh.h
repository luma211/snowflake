/*
* file BasicStaticMesh.h
*
* author luoxw
* date 2017/02/10
*
*
*/

#pragma once

#include "BasicInterface.h"

namespace LostCore
{
	class IPrimitiveGroup;

	class FBasicStaticMesh : public IBasicInterface
	{
	public:
		FBasicStaticMesh();
		virtual ~FBasicStaticMesh() override;

		virtual bool Init(const char* name, IRenderContext * rc) override;
		virtual void Fini() override;
		virtual void Tick(float sec) override;
		virtual void Draw(float sec, IRenderContext * rc) override;

	public:
		virtual void AddPrimitiveGroup(IPrimitiveGroup* pg);
		virtual void RemovePrimitiveGroup(IPrimitiveGroup* pg);
		virtual void ClearPrimitiveGroup(std::function<void(IPrimitiveGroup*)> func);

	protected:
		std::vector<IPrimitiveGroup*> PrimitiveGroups;
	};
}