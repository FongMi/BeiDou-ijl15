#pragma once
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")

class ConvertUTF8
{
public:
	static bool Hook();
private:
	static bool Hook_PeekMessageA();
	static bool Hook_ImmAssociateContext();
	static bool Hook_GetTextExtentPoint32A();
	static bool Hook_MultiByteToWideChar();
	static bool Hook_WideCharToMultiByte();
	static bool Hook_TextOutA();
	static bool Hook_CharPrevA();
	static bool Hook_CharNextA();
};