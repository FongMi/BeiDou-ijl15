#pragma once
#include <imm.h>
#pragma comment(lib, "imm32.lib")

BYTE enabled = 1;

void disableIme() {
	HWND hwnd = GetForegroundWindow();
	if (hwnd) {
		HIMC hImc = ImmGetContext(hwnd);
		if (hImc) {
			ImmAssociateContext(hwnd, NULL);
			ImmReleaseContext(hwnd, hImc);
		}
	}
}

DWORD funcEnableImeAddr = 0x009E85F3;
DWORD switchImeRtnAddr = 0x004CA08F;
__declspec(naked) void switchIme() {
	__asm {
		cmp[esi + 0x80], 1
		jz label_disable
		push 1
		call funcEnableImeAddr
		mov enabled, 1
		jmp switchImeRtnAddr

		label_disable :
		call disableIme
			jmp switchImeRtnAddr
	}
}

DWORD destroyWindowRtnAddr = 0x004DFEAD;
DWORD destroyWindowFuncAddr = 0x0041FE69;
__declspec(naked) void destroyWindow() {
	__asm {
		call destroyWindowFuncAddr
		or dword ptr[esi + 14h], 0FFFFFFFFh

		cmp enabled, 0
		jz label_return

		call disableIme
		mov enabled, 0

		label_return :
		jmp destroyWindowRtnAddr
	}
}

DWORD switchMLImeRtnAddr = 0x004D32EE;
__declspec(naked) void switchMLIme() {
	__asm {
		push 1
		call funcEnableImeAddr
		mov enabled, 1
		jmp  switchMLImeRtnAddr
	}
}

class FixIme {
public:
	static void Hook() {
		GeneralHook();
		Memory::CodeCave(switchIme, 0x004CA089, 6);
		Memory::CodeCave(destroyWindow, 0x004DFEA4, 9);
		Memory::CodeCave(switchMLIme, 0x004D32D9, 7);
	}
private:
	static void GeneralHook() {
		Memory::FillBytes(0x008D54A6, 0x90, 9); // Key
		Memory::FillBytes(0x00937225, 0x90, 9); // Chat
		Memory::FillBytes(0x00531EE8, 0x90, 9); // Group Message
		Memory::FillBytes(0x004CAE7D, 0x90, 2);
		Memory::WriteByte(0x004CAE8F, 0xEB);
	}
};