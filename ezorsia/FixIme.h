#pragma once
#include <imm.h>
#pragma comment(lib, "imm32.lib")

void EnableIme() {
	HWND hwnd = GetForegroundWindow();
	if (hwnd) {
		HIMC hImc = ImmGetContext(hwnd);
		if (hImc) {
			ImmAssociateContext(hwnd, hImc);
			ImmReleaseContext(hwnd, hImc);
		}
	}
}

void DisableIme() {
	HWND hwnd = GetForegroundWindow();
	if (hwnd) {
		HIMC hImc = ImmGetContext(hwnd);
		if (hImc) {
			ImmAssociateContext(hwnd, NULL);
			ImmReleaseContext(hwnd, hImc);
		}
	}
}

BYTE enabled = 1;

DWORD funcEnableImeAddr = 0x009E85F3;

DWORD setOnFocusFirstJudgementRtnAddr = 0x004CA061;
DWORD switchImeAddr = 0x004CA078;
__declspec(naked) void setOnFocusFirstJudgement() {
	__asm {
		cmp[esp + 0Ch], edi
		jz label_jmp_switch_ime
		jmp setOnFocusFirstJudgementRtnAddr
		label_jmp_switch_ime :
		jmp switchImeAddr
	}
}

DWORD enableRtnAddr = 0x004CA08F;
DWORD disableRtnAddr = 0x004CA091;
__declspec(naked) void switchIme() {
	__asm {
		cmp[esp + 0Ch], edi
		jz  label_jz
		xor eax, eax
		cmp[esi + 0x80], eax
		setz al
		push eax
		call funcEnableImeAddr
		mov enabled, 1
		jmp  enableRtnAddr

		label_jz :
		push 0
		call funcEnableImeAddr
		jmp  disableRtnAddr
	}
}

DWORD enableMLRtnAddr = 0x004D32E0;
DWORD disableMLRtnAddr = 0x004D32E2;
__declspec(naked) void switchMLIme() {
	__asm {
		cmp  dword ptr[esp + 8], 0
		jz   label_jz
		push 1
		call funcEnableImeAddr
		mov enabled, 1
		jmp  enableMLRtnAddr

		label_jz :
		push 0
		call funcEnableImeAddr
		jmp  disableMLRtnAddr
	}
}

DWORD newSwitchImeRtnAddr = 0x004CA08F;
__declspec(naked) void newSwitchIme() {
	__asm {
		cmp[esi + 0x80], 1
		jz label_disable
		push 1
		call funcEnableImeAddr
		mov enabled, 1
		jmp newSwitchImeRtnAddr

		label_disable :
		call DisableIme
		jmp newSwitchImeRtnAddr
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

		call DisableIme
		mov enabled, 0

		label_return :
		jmp destroyWindowRtnAddr
	}
}

DWORD newSwitchMLImeRtnAddr = 0x004D32EE;
__declspec(naked) void newSwitchMLIme() {
	__asm {
		push 1
		call funcEnableImeAddr
		mov enabled, 1
		jmp  newSwitchMLImeRtnAddr
	}
}


class FixIme {
public:
	static void HookOld() {
		GeneralHook();
		Memory::CodeCave(setOnFocusFirstJudgement, 0x004CA05B, 6);
		Memory::CodeCave(switchIme, 0x004CA089, 6);
		Memory::FillBytes(0x004D32C6, 0x90, 2);
		Memory::CodeCave(switchMLIme, 0x004D32D9, 7);
		Memory::CodeCave(destroyWindow, 0x004DFEA4, 9);
		std::cout << "Old Ime Hook" << std::endl;
	}

	static void HookNew() {
		GeneralHook();
		Memory::CodeCave(newSwitchIme, 0x004CA089, 6);
		Memory::CodeCave(destroyWindow, 0x004DFEA4, 9);
		Memory::CodeCave(newSwitchMLIme, 0x004D32D9, 7);
		std::cout << "New Ime Hook" << std::endl;
	}
private:
	static void GeneralHook() {
		Memory::FillBytes(0x008D54A6, 0x90, 9); // Key ?
		Memory::FillBytes(0x00937225, 0x90, 9); // Chat
		Memory::FillBytes(0x00531EE8, 0x90, 9); // Group Message
		Memory::FillBytes(0x004CAE7D, 0x90, 2);
		Memory::WriteByte(0x004CAE8F, 0xEB);
		Memory::FillBytes(0x007A015D, 0x90, 2);
	}
};