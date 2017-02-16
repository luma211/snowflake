/*
* file LostCore.h
*
* author luoxw
* date 2017/02/04
*
*
*/

#pragma once

#ifdef LOSTCORE_EXPORTS
#define LOSTCORE_API __declspec(dllexport)
#else
#define LOSTCORE_API __declspec(dllimport)
#endif

namespace LostCore
{
	/*********************************************************************/

	static const char* SModuleName = "LostCore.dll";
	static HMODULE SModuleHandle = NULL;

	//************************************
	// Method:    StartupModule ����dll
	// FullName:  D3D11::StartupModule
	// Access:    public 
	// Returns:   
	// Qualifier:
	// Parameter: const char* path dll�ļ���·��
	//************************************
	static void StartupModule(const char* directory)
	{
		if (SModuleHandle != NULL)
		{
			return;
		}

		char dllPath[MAX_PATH];
		snprintf(dllPath, MAX_PATH, "%s%s", directory, SModuleName);
		SModuleHandle = LoadLibraryA(dllPath);

		if (SModuleHandle == NULL)
		{
			MessageBoxA(NULL, "failed to load dll", dllPath, 0);
			return;
		}
	}

	//************************************
	// Method:    ShutdownModule �ͷ�dll
	// FullName:  D3D11::ShutdownModule
	// Access:    public 
	// Returns:   
	// Qualifier:
	//************************************
	static void ShutdownModule()
	{
		if (SModuleHandle == NULL)
		{
			return;
		}

		FreeLibrary(SModuleHandle);
		SModuleHandle = NULL;
	}

	static HMODULE GetModule_LostCore()
	{
		if (SModuleHandle == NULL)
		{
			StartupModule("");
		}

		return SModuleHandle;
	}
}

#include "ExportTest.h"
#include "SimpleWorld.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quat.h"
#include "Math/Matrix.h"
#include "Math/Transform.h"

namespace LostCore
{
}