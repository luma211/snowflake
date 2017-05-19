/*
* file SimpleWorld.cpp
*
* author luoxw
* date 2017/02/05
*
*
*/

#include "stdafx.h"
#include "SimpleWorld.h"
#include "BasicModel.h"
#include "BasicCamera.h"
#include "BasicScene.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

namespace LostCore
{
	class FSimpleCamera : public FBasicCamera
	{
	public:
		FSimpleCamera();
		virtual ~FSimpleCamera();

		bool Load(IRenderContext * rc, const char* url) override;
		virtual void Draw(IRenderContext * rc, float sec) override;
	};

	FSimpleCamera::FSimpleCamera()
	{
	}

	FSimpleCamera::~FSimpleCamera()
	{
	}

	bool FSimpleCamera::Load(IRenderContext * rc, const char* url)
	{
		return true;
	}

	void FSimpleCamera::Draw(IRenderContext * rc, float sec)
	{
		if (rc != nullptr)
		{
			rc->SetViewProjectMatrix(GetViewProjectMatrix());
		}
	}

	/*
	class FSimpleScene : public FBasicScene
	{
	public:
		FSimpleScene();
		virtual ~FSimpleScene() override;

		virtual void Draw(IRenderContext * rc, float sec) override;
		bool Load(IRenderContext * rc, const char* url) override;
		virtual void Fini() override;

	private:
		IPrimitiveGroup*	APrimitiveGroup;
		IMaterial*			AMaterial;
	};

	FSimpleScene::FSimpleScene()
		: APrimitiveGroup(nullptr)
		, AMaterial(nullptr)
	{
	}

	FSimpleScene::~FSimpleScene()
	{
		//assert(APrimitiveGroup == nullptr);
		//assert(AMaterial == nullptr);
	}

	void FSimpleScene::Draw(IRenderContext * rc, float sec)
	{
		FTransform world(FVec3(0.f, 0.f, 5.f));
		FMatrix mat = world.ToMatrix();
		AMaterial->UpdateConstantBuffer(rc, (const void*)&mat, sizeof(FMatrix));

		FBasicScene::Draw(rc, sec);
	}

	bool FSimpleScene::Load(IRenderContext * rc, const char* url)
	{
		// hard coded static mesh creation.
		FBasicModel* geo = new FBasicModel;
		AddModel(geo);

		if (SSuccess != WrappedCreatePrimitiveGroup(&APrimitiveGroup))
		{
			return false;
		}

		__declspec(align(16)) struct _Vertex { FVec3 RGB; FVec3 XYZ; FVec3 N; FVec2 UV; };
		float scaler = 0.5f;

		// Mesh vertices
		const _Vertex vertices[] =
		{
			{ FVec3(1.f, 0.f, 0.f),	FVec3(0.f, 0.f, -scaler),	FVec3(0.5f, -0.5f, -0.5f),	FVec2(0.0f, 1.0f) },		// center
			{ FVec3(1.f, 0.f, 0.f),	FVec3(0.f, scaler, 0.f),	FVec3(0.5f, -0.5f, -0.5f),	FVec2(0.0f, 0.0f) },		// top
			{ FVec3(1.f, 0.f, 0.f),	FVec3(scaler, -scaler, 0.f),FVec3(0.5f, -0.5f, -0.5f),	FVec2(1.0f, 0.0f) },		// bottom right

			{ FVec3(0.f, 0.f, 0.f),	FVec3(0.f, scaler, 0.f),	FVec3(0.f, 0.f, 1.f),		FVec2(0.0f, 0.0f) },		// top
			{ FVec3(0.f, 0.f, 0.f),	FVec3(-scaler, -scaler, 0), FVec3(0.f, 0.f, 1.f),		FVec2(1.0f, 1.0f) },		// bottom left
			{ FVec3(0.f, 0.f, 0.f),	FVec3(scaler, -scaler, 0.f),FVec3(0.f, 0.f, 1.f),		FVec2(1.0f, 0.0f) },		// bottom right

			{ FVec3(0.f, 1.f, 0.f),	FVec3(0.f, scaler, 0.f),	FVec3(-0.5f, -0.5f, -0.5f),	FVec2(0.0f, 0.0f) },		// top
			{ FVec3(0.f, 1.f, 0.f),	FVec3(0.f, 0.f, -scaler),	FVec3(-0.5f, -0.5f, -0.5f), FVec2(0.0f, 1.0f) },		// center
			{ FVec3(0.f, 1.f, 0.f),	FVec3(-scaler, -scaler, 0), FVec3(-0.5f, -0.5f, -0.5f), FVec2(1.0f, 1.0f) },		// bottom left

			{ FVec3(0.f, 0.f, 1.f),	FVec3(-scaler, -scaler, 0), FVec3(0.f, -0.5f, -0.5f),	FVec2(1.0f, 1.0f) },		// bottom left
			{ FVec3(0.f, 0.f, 1.f),	FVec3(0.f, 0.f, -scaler),	FVec3(0.f, -0.5f, -0.5f),	FVec2(0.0f, 1.0f) },		// center
			{ FVec3(0.f, 0.f, 1.f),	FVec3(scaler, -scaler, 0.f),FVec3(0.f, -0.5f, -0.5f),	FVec2(1.0f, 0.0f) },		// bottom right
		};

		// Mesh indices
		const uint16 indices[] = { 0, 1, 2, 1, 3, 2, 3, 1, 0, 3, 0, 2 };

		if (!APrimitiveGroup->ConstructVB(rc, vertices, sizeof(vertices), sizeof(_Vertex), false))
			//||
			//!APrimitiveGroup->ConstructIB(rc, indices, sizeof(indices), sizeof(uint16), false))
		{
			return false;
		}

		if (AMaterial != nullptr)
		{
			WrappedDestroyMaterial_SceneObject(std::forward<IMaterial*>(AMaterial));
		}

		if (SSuccess != WrappedCreateMaterial_SceneObject(&AMaterial) ||
			!AMaterial->Initialize(rc, "dummy_normal.json"))
		{
			return false;
		}

		APrimitiveGroup->SetMaterial(AMaterial);
		geo->AddPrimitiveGroup(APrimitiveGroup);
		return true;
	}

	void FSimpleScene::Fini()
	{
		ClearStaticMesh([](FBasicModel* p) 
		{ 
			if (p != nullptr)
			{
				p->Fini();
				delete p;
			}
		});

		//if (APrimitiveGroup != nullptr)
		//{
		//	WrappedDestroyPrimitiveGroup(std::forward<IPrimitiveGroup*>(APrimitiveGroup));
		//	APrimitiveGroup = nullptr;
		//}

		//if (AMaterial != nullptr)
		//{
		//	WrappedDestroyMaterial(std::forward<IMaterial*>(AMaterial));
		//	AMaterial = nullptr;
		//}
	}
	*/

	class FSimpleWorld : public FBasicWorld
	{
	public:
		FSimpleWorld();
		virtual ~FSimpleWorld() override;

		bool Load(IRenderContext * rc, const char* url) override;
		virtual void Fini() override;
		virtual void DrawPreScene(float sec) override;

		virtual bool InitWindow(const char* name, HWND wnd, bool bWindowed, int32 width, int32 height) override;
		virtual IRenderContext* GetRenderContext() override;
		virtual FBasicCamera* GetCamera() override;

	private:
		IRenderContext*			RC;
		FSimpleCamera*			Camera;
	};

	FSimpleWorld::FSimpleWorld() : RC(nullptr), Camera(nullptr)
	{
	}

	FSimpleWorld::~FSimpleWorld()
	{
		assert(RC == nullptr);
		assert(Camera == nullptr);
	}

	bool FSimpleWorld::Load(IRenderContext * rc, const char* url)
	{
		FBasicWorld::Load(rc, "");

		Camera = new FSimpleCamera;
		if (!Camera->Load(rc, ""))
		{
			return false;
		}

		auto scene = new FBasicScene;
		if (scene->Load(rc, "default_empty.json"))
		{
			AddScene(scene);
			return true;
		}
		else
		{
			return false;
		}
	}
	 
	void FSimpleWorld::Fini()
	{
		FBasicWorld::Fini();

		ClearScene([](FBasicScene* p)
		{ 
			if (p != nullptr)
			{
				p->Fini();
				delete p;
			} 
		});

		if (RC != nullptr)
		{
			RC->Fini();
			delete RC;
			RC = nullptr;
		}

		if (Camera != nullptr)
		{
			Camera->Fini();
			delete Camera;
			Camera = nullptr;
		}
	}

	void FSimpleWorld::DrawPreScene(float sec)
	{
		FBasicWorld::DrawPreScene(sec);

		if (RC == nullptr)
		{
			return;
		}

		if (Camera == nullptr)
		{
			return;
		}

		Camera->Draw(RC, sec);
	}

	bool FSimpleWorld::InitWindow(const char* name, HWND wnd, bool bWindowed, int32 width, int32 height)
	{
		auto ret = WrappedCreateRenderContext(EContextID::D3D11_DXGI0, &RC);
		assert(SSuccess == ret);
		if (RC != nullptr && RC->Init(wnd, bWindowed, width, height) && Load(RC, ""))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	IRenderContext * FSimpleWorld::GetRenderContext()
	{
		return RC;
	}

	FBasicCamera * FSimpleWorld::GetCamera()
	{
		return Camera;
	}

	LOSTCORE_API void CreateSimpleWorld(FBasicWorld ** ppObj)
	{
		if (ppObj != nullptr)
		{
			*ppObj = new FSimpleWorld;
		}
	}

	LOSTCORE_API void DestroySimpleWorld(FBasicWorld * obj)
	{
		if (obj != nullptr)
		{
			delete obj;
		}
	}
}