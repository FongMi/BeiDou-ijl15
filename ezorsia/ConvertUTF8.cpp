#include "stdafx.h"
#include "ConvertUTF8.h"

bool ConvertUTF8::Hook_PeekMessageA()
{
	static auto pPeekMessageA = decltype(&PeekMessageA)(Memory::GetFunctionAddress("USER32", "PeekMessageA"));

	decltype(&PeekMessageA) Hook = [](LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) -> BOOL
	{
		return PeekMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	};

	return Memory::SetHook(true, reinterpret_cast<void**>(&pPeekMessageA), Hook);
}

bool ConvertUTF8::Hook_ImmAssociateContext()
{
	static auto _ImmAssociateContext = decltype(&ImmAssociateContext)(Memory::GetFunctionAddress("IMM32", "ImmAssociateContext"));

	decltype(&ImmAssociateContext) Hook = [](HWND hWnd, HIMC hIMC) -> HIMC
	{
		HIMC overrideHIMC = ImmGetContext(hWnd);
		ImmSetOpenStatus(overrideHIMC, TRUE);
		return _ImmAssociateContext(hWnd, overrideHIMC);;
	};

	return Memory::SetHook(true, reinterpret_cast<void**>(&_ImmAssociateContext), Hook);
}

bool ConvertUTF8::Hook_GetTextExtentPoint32A()
{
	static auto pGetTextExtentPoint32A = decltype(&GetTextExtentPoint32A)(Memory::GetFunctionAddress("GDI32", "GetTextExtentPoint32A"));

	decltype(&GetTextExtentPoint32A) Hook = [](HDC hdc, LPCSTR lpString, int c, LPSIZE psizl) -> BOOL
	{
		wchar_t* wideStr = new wchar_t[2 * c + +2];
		int len = MultiByteToWideChar(CP_UTF8, 0, lpString, c, wideStr, c);
		wideStr[len] = '\0';
		return GetTextExtentPoint32W(hdc, wideStr, len, psizl);
	};

	return Memory::SetHook(true, reinterpret_cast<void**>(&pGetTextExtentPoint32A), Hook);
}

bool ConvertUTF8::Hook_MultiByteToWideChar()
{
	static auto _MultiByteToWideChar = decltype(&MultiByteToWideChar)(Memory::GetFunctionAddress("KERNEL32", "MultiByteToWideChar"));

	decltype(&MultiByteToWideChar) Hook = [](UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar) -> int
	{
		CodePage = CP_UTF8;
		return _MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
	};

	return Memory::SetHook(true, reinterpret_cast<void**>(&_MultiByteToWideChar), Hook);
}

bool ConvertUTF8::Hook_WideCharToMultiByte()
{
	static auto _WideCharToMultiByte = decltype(&WideCharToMultiByte)(Memory::GetFunctionAddress("KERNEL32", "WideCharToMultiByte"));

	decltype(&WideCharToMultiByte) Hook = [](UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, PCCH lpDefaultChar, LPBOOL lpUsedDefaultChar) -> int
	{
		CodePage = CP_UTF8;
		int result = _WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
		return result;
	};

	return Memory::SetHook(true, reinterpret_cast<void**>(&_WideCharToMultiByte), Hook);
}

bool ConvertUTF8::Hook_TextOutA()
{
	static auto pTextOutA = decltype(&TextOutA)(Memory::GetFunctionAddress("GDI32", "TextOutA"));

	decltype(&TextOutA) Hook = [](HDC hdc, int x, int y, LPCSTR lpString, int c) -> BOOL
	{
		BOOL result = FALSE;
		wchar_t* wideStr = new wchar_t[2 * c + 2];
		int len = MultiByteToWideChar(CP_UTF8, 0, lpString, c, wideStr, c);
		wideStr[len] = '\0';
		return TextOutW(hdc, x, y, wideStr, len);
	};

	return Memory::SetHook(true, reinterpret_cast<void**>(&pTextOutA), Hook);
}

bool ConvertUTF8::Hook_CharPrevA()
{
	static auto pCharPrevA = decltype(&CharPrevA)(Memory::GetFunctionAddress("USER32", "CharPrevA"));

	decltype(&CharPrevA) Hook = [](LPCSTR lpszStart, LPCSTR lpszCurrent) -> LPSTR
	{
		LPCSTR result = lpszCurrent;
		char pCurrentChar;
		do {
			if (result <= lpszStart)
				break;
			pCurrentChar = *--result;
		} while ((pCurrentChar & 0xC0) == 0x80);
		return (LPSTR)result;
	};

	return Memory::SetHook(true, reinterpret_cast<void**>(&pCharPrevA), Hook);
}

bool ConvertUTF8::Hook_CharNextA()
{
	static auto pCharNextA = decltype(&CharNextA)(Memory::GetFunctionAddress("USER32", "CharNextA"));

	decltype(&CharNextA) Hook = [](LPCSTR lpCurrentChar) -> LPSTR
	{
		LPCSTR result = lpCurrentChar;
		char pCurrentChar;
		if (*result) {
			do {
				pCurrentChar = *++result;
			} while ((pCurrentChar & 0xC0) == 0x80);
		}
		return (LPSTR)result;

	};

	return Memory::SetHook(true, reinterpret_cast<void**>(&pCharNextA), Hook);
}

bool ConvertUTF8::Hook()
{
	//修正 IME 輸入後無法送出
	Memory::PatchNop(0x008D54A6, 9); // Key ?
	Memory::PatchNop(0x00937225, 9); // Chat
	Memory::PatchNop(0x00531EE8, 9); // Group Message

	// 剪貼板支援中文
	Memory::PatchNop(0x004CAE7D, 2);
	Memory::WriteByte(0x004CAE8F, 0xEB);

	bool bResult = true;
	bResult &= ConvertUTF8::Hook_WideCharToMultiByte();
	bResult &= ConvertUTF8::Hook_MultiByteToWideChar();
	bResult &= ConvertUTF8::Hook_CharNextA();
	bResult &= ConvertUTF8::Hook_CharPrevA();
	bResult &= ConvertUTF8::Hook_TextOutA();
	bResult &= ConvertUTF8::Hook_GetTextExtentPoint32A();
	bResult &= ConvertUTF8::Hook_ImmAssociateContext();
	bResult &= ConvertUTF8::Hook_PeekMessageA();
	return bResult;
}