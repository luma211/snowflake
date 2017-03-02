/*
* file MaterialInterface.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once

#include "Drawable.h"

namespace LostCore
{

	class IMaterial : public IDrawable
	{
	public:
		virtual bool Initialize(LostCore::IRenderContext * rc, const char* path) = 0;
		virtual void UpdateMatrix_World(LostCore::IRenderContext * rc, const FMatrix& mat) = 0;
	};
}