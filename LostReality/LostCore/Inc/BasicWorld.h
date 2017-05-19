/*
* file BasicWorld.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once

#include "BasicInterface.h"
#include "BasicScene.h"
#include "BasicGUI.h"

namespace LostCore
{
	class FBasicCamera;

	class FBasicWorld : public IBasicInterface
	{
	public:
		FBasicWorld();
		virtual ~FBasicWorld() override;

		virtual bool Load(IRenderContext * rc, const char* url) override;
		virtual void Fini() override;
		virtual void Tick(float sec) override;
		virtual void Draw(IRenderContext * rc, float sec) override;

		virtual void DrawPreScene(float sec);
		virtual void DrawPostScene(float sec);

		virtual bool InitWindow(const char* name, HWND wnd, bool bWindowed, int32 width, int32 height);
		virtual IRenderContext* GetRenderContext();
		virtual FBasicCamera* GetCamera();

		virtual void AddScene(FBasicScene * scene);
		virtual void RemoveScene(FBasicScene * scene);
		virtual void ClearScene(std::function<void(FBasicScene *)> func);

	private:
		std::vector<FBasicScene*> SceneArray;
		FBasicGUI* GUIRoot;
	};
}