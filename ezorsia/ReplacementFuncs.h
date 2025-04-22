#pragma once
#include "AutoTypes.h"
#include "ReplacementString.h"

bool HookGetModuleFileName(bool bEnable) {
	static decltype(&GetModuleFileNameW) _GetModuleFileNameW = &GetModuleFileNameW;

	const decltype(&GetModuleFileNameW) GetModuleFileNameW_Hook = [](HMODULE hModule, LPWSTR lpFileName, DWORD dwSize) -> DWORD {
		auto len = _GetModuleFileNameW(hModule, lpFileName, dwSize);
		// Check to see if the length is invalid (zero)
		if (!len) {
			// Try again without the provided module for a fixed result
			len = _GetModuleFileNameW(nullptr, lpFileName, dwSize);
		}
		return len;
	};

	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_GetModuleFileNameW), GetModuleFileNameW_Hook);
}

/// <summary>
/// Creates a detour for the User32.dll CreateWindowExA function applying the following changes:
/// 1. Enable the window minimize box
/// </summary>
inline void HookCreateWindowExA(bool bEnable) {
	static auto create_window_ex_a = decltype(&CreateWindowExA)(GetProcAddress(LoadLibraryA("USER32"), "CreateWindowExA"));
	static const decltype(&CreateWindowExA) hook = [](DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) -> HWND {
		dwStyle |= WS_MINIMIZEBOX; // enable minimize button
		return create_window_ex_a(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	};
	Memory::SetHook(bEnable, reinterpret_cast<void**>(&create_window_ex_a), hook);
}

DWORD GetFuncAddress(LPCSTR lpModule, LPCSTR lpFunc)	//ty alias!			//multiclient, not currently working, likely cannot hook early enough with nmconew.dll
{
	HMODULE mod = LoadLibraryA(lpModule);

	if (!mod)
	{
		return 0;
	}

	DWORD address = (DWORD)GetProcAddress(mod, lpFunc);

	if (!address)
	{
		return 0;
	}

	return address;
}

bool Hook_CreateMutexA(bool bEnable)	//ty darter	//ty angel!
{
	static auto _CreateMutexA = decltype(&CreateMutexA)(GetFuncAddress("KERNEL32", "CreateMutexA"));

	decltype(&CreateMutexA) Hook = [](LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName) -> HANDLE
	{
		//Multi-Client Check Removal
		if (lpName && strstr(lpName, "WvsClientMtx"))
		{
			return (HANDLE)0x0BADF00D;
			//char szMutex[128];
			//lpName = szMutex;
		}

		return _CreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
	};

	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_CreateMutexA), Hook);
}

bool HookPcCreateObject_IWzResMan(bool bEnable)
{
	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_PcCreateObject_IWzResMan), _PcCreateObject_IWzResMan_Hook);
}
bool HookPcCreateObject_IWzNameSpace(bool bEnable)
{
	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_PcCreateObject_IWzNameSpace), _PcCreateObject_IWzNameSpace_Hook);
}
bool HookPcCreateObject_IWzFileSystem(bool bEnable)
{
	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_PcCreateObject_IWzFileSystem), _PcCreateObject_IWzFileSystem_Hook);
}
bool HookCWvsApp__Dir_BackSlashToSlash(bool bEnable)
{
	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_CWvsApp__Dir_BackSlashToSlash), _CWvsApp__Dir_BackSlashToSlash_Hook);
}
bool HookCWvsApp__Dir_upDir(bool bEnable)
{
	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_CWvsApp__Dir_upDir), _CWvsApp__Dir_upDir_Hook);
}
bool Hookbstr_ctor(bool bEnable)
{
	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_bstr_ctor), _bstr_ctor_Hook);
}
bool HookIWzFileSystem__Init(bool bEnable)
{
	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_IWzFileSystem__Init), _IWzFileSystem__Init_Hook);
}
bool HookIWzNameSpace__Mount(bool bEnable)
{
	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_IWzNameSpace__Mount), _IWzNameSpace__Mount_Hook);
}

bool HookCWvsApp__InitializeResMan(bool bEnable)	//resman hook that does nothing, kept for analysis and referrence //not skilled enough to rewrite to load custom wz files
{
	static _CWvsApp__InitializeResMan_t _CWvsApp__InitializeResMan_Hook = [](void* pThis, void* edx) {
		_CWvsApp__InitializeResMan(pThis, edx);	//comment this out and uncomment below if testing, supposed to load from .img files in folders but i never got to test it
	};
	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_CWvsApp__InitializeResMan), _CWvsApp__InitializeResMan_Hook);
}

bool Hook_StringPool__GetString(bool bEnable)	//hook stringpool modification //ty !! popcorn //ty darter
{
	_StringPool__GetString_t _StringPool__GetString_Hook = [](void* pThis, void* edx, ZXString<char>* result, unsigned int nIdx, char formal) ->  ZXString<char>*
	{
		auto ret = _StringPool__GetString(pThis, edx, result, nIdx, formal);
		for (int i = 0; i < keyValuePairsCount; ++i) {
			if (nIdx == keyValuePairs[i].key) {
				*ret = keyValuePairs[i].value.c_str();
				break;
			}
		}
		return ret;
	};
	return Memory::SetHook(bEnable, reinterpret_cast<void**>(&_StringPool__GetString), _StringPool__GetString_Hook);
}