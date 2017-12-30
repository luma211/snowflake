/*
* file ListBox.h
*
* author luoxw
* date 2017/10/15
*
*
*/

#pragma once

#include "Inc/BasicGUI.h"

namespace LostCore
{
	class FTextBox : public FRect
	{
	public:
		FTextBox();
		virtual ~FTextBox() override;

		virtual void Update() override;

		void SetText(const string& text);
		void SetText(const wstring& text);

	private:
		wstring content;
		int32 Space;
	};
}

