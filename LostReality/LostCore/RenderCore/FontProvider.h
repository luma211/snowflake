/*
* file FontProvider.h
*
* author luoxw
* date 2017/04/25
*
*
*/

#pragma once

#include "FontInterface.h"

namespace LostCore
{
	// FBasicGUI负责这个单例的初始化和释放.
	class FFontProvider
	{
	public:
		static FFontProvider* Get()
		{
			static FFontProvider Inst;
			return &Inst;
		}

		FFontProvider() : GdiFont(nullptr) {}

		void Init(IRenderContext * rc);
		void Fini();

		IFontInterface* GetGdiFont();

	private:
		IFontInterface* GdiFont;
	};
}

