#pragma once
DWORD fixMouseWheelAddr = 0x009E8090;
DWORD fixMouseWheelRetJmpAddr = 0x009E809F;
DWORD fixMouseWheelCallSetCursorPosAddr = 0x0059A0CB;
__declspec(naked) void fixMouseWheelHook() {
	__asm {
		// is mouse wheel
		cmp eax, 522
		je next
		mov eax, dword ptr ds : [edi]
		shr eax, 0x10
		push eax
		movzx eax, word ptr ds : [edi]
		push eax
		call fixMouseWheelCallSetCursorPosAddr
		next :
		jmp[fixMouseWheelRetJmpAddr]
	}
}

DWORD fixDateFormatRtnAddr = 0x008EBF65;
__declspec(naked) void fixDateFormat() {
	__asm {
		movzx   ecx, word ptr[ebp - 16h]
		push    ecx
		movzx   ecx, word ptr[ebp - 1Ah]
		push    ecx
		movzx   ecx, word ptr[ebp - 1Ch]
		jmp fixDateFormatRtnAddr
	}
}
DWORD fixDateFormat2RtnAddr = 0x008EBFAF;
__declspec(naked) void fixDateFormat2() {
	__asm {
		movzx   ecx, word ptr[ebp - 16h]
		push    ecx
		movzx   ecx, word ptr[ebp - 1Ah]
		push    ecx
		movzx   ecx, word ptr[ebp - 1Ch]
		jmp fixDateFormat2RtnAddr
	}
}
DWORD fixDateFormat3RtnAddr = 0x008EC328;
__declspec(naked) void fixDateFormat3() {
	__asm {
		movzx   ecx, word ptr[ebp - 1Eh]
		push    ecx
		movzx   ecx, word ptr[ebp - 22h]
		push    ecx
		movzx   ecx, word ptr[ebp - 24h]
		jmp fixDateFormat3RtnAddr
	}
}
DWORD fixDateFormat4RtnAddr = 0x008EBF13;
__declspec(naked) void fixDateFormat4() {
	__asm {
		movzx   ecx, word ptr[ebp - 16h]
		push    ecx
		movzx   ecx, word ptr[ebp - 1Ah]
		push    ecx
		movzx   ecx, word ptr[ebp - 1Ch]
		jmp fixDateFormat4RtnAddr
	}
}

DWORD getItemType2Addr = 0x005CFAC2;
__declspec(naked) void getItemType1() {
	__asm {
		jmp getItemType2Addr
	}
}

DWORD getItemType2ErrRtnAddr = 0x005CFAA8;
DWORD getItemType2RtnAddr = 0x005CFADD;
__declspec(naked) void getItemType2() {
	__asm {
		dec eax
		jz label_eqp
		dec eax
		jz label_use
		dec eax
		jz label_ins
		dec eax
		jz label_etc
		dec eax
		jz label_cash
		jmp getItemType2ErrRtnAddr
	label_cash:
		push 0x159C
		jmp getItemType2RtnAddr
	label_etc:
		push 0x6DD
		jmp getItemType2RtnAddr
	label_ins:
		push 0x0B
		jmp getItemType2RtnAddr
	label_use:
		push 0x6E3
		jmp getItemType2RtnAddr
	label_eqp:
		push 0x6D9
		jmp getItemType2RtnAddr
	}
}

const DWORD back1 = 0x007807A1;
__declspec(naked) void customJumpCapHook1()
{
	__asm {
		cmp eax, Client::jumpCap
		jl label
		push Client::jumpCap
		pop eax
	label:
		mov edx, edi
		jmp[back1]
	}
}


const DWORD back2 = 0x008C42AD;
const DWORD back3 = 0x008C42AF;
__declspec(naked) void customJumpCapHook2()
{
	__asm {
		cmp eax, Client::jumpCap
		jle label
		push Client::jumpCap
		pop edi
		jmp[back3]
		label:
		jmp[back2]
	}
}

const DWORD back4 = 0x0094D947;
__declspec(naked) void customJumpCapHook3()
{
	__asm {
		push Client::jumpCap
		pop ecx
		cmp eax, ecx
		jmp[back4]
	}
}

int curSpeed = 100;
void calcClimbSpeed() {
	int speed = curSpeed;
	speed = speed < 80 ? 80 : speed;
	speed = speed > Client::speedMovementCap ? Client::speedMovementCap : speed;

	double climbingSpeed = Client::climbSpeed;
	climbingSpeed = climbingSpeed <= 1.0 ? 1.0 : climbingSpeed;
	double curClimbSpeed = 3.0 * speed * climbingSpeed / 100;
	Memory::WriteDouble(0x00C1CF80, curClimbSpeed);
}

DWORD calcSpeedHookRtn = 0x0094D942;
__declspec(naked) void calcSpeedHook()
{
	__asm {
		push eax
		mov eax, [ebp - 10h]
		mov curSpeed, eax
		call calcClimbSpeed
		pop eax
		cmp     eax, edi
		jg label_return
		mov     eax, edi

		label_return :
		jmp calcSpeedHookRtn
	}
}

int charLen = 55;
void calcCharLen(const char* word) {
	const std::string str(word);
	int width = 0, pos = 0, len = str.size();
	while (pos < len && width < 55) {
		unsigned char c = str[pos];
		int bytes = 1, w = 1;
		if ((c & 0xE0) == 0xC0)      bytes = 2, w = 2;
		else if ((c & 0xF0) == 0xE0) bytes = 3, w = 2;
		else if ((c & 0xF8) == 0xF0) bytes = 4, w = 2;
		if (width + w > 55) break;
		width += w;
		pos += bytes;
	}
	charLen = pos;
}

constexpr DWORD skillToolTipNewRtn = 0x008F3844;
__declspec(naked) void skillToolTip()
{
	__asm {
		mov eax, [ebp + 0Ch]
		push eax
		call calcCharLen
		pop eax
		mov eax, charLen
		mov[ebp - 1Ch], eax
		lea eax, [ebp - 30h]
		jmp skillToolTipNewRtn
	}
}

const DWORD chatTextPosRtn = 0x008DD075;
__declspec(naked) void chatTextPos()
{
	__asm {
		add eax, [edi + 0CFCh]
		cmp[edi + 0D00h], 3
		jz label_type3
		cmp[edi + 0D00h], 2
		jz label_type2
		sub eax, 1
			jmp label_rtn
			label_type2 :
		jmp label_rtn
			label_type3 :
		sub eax, 2
			label_rtn :
			jmp chatTextPosRtn
	}
}

const DWORD mbpos1Rtn = 0x0086437D;
__declspec(naked) void mbpos1()
{
	__asm {
		push 133
		sub eax, edi
		push eax
		jmp mbpos1Rtn
	}
}

const DWORD mbpos2Rtn = 0x0086449A;
__declspec(naked) void mbpos2()
{
	__asm {
		push 165
		sub eax, edi
		push eax
		jmp mbpos2Rtn
	}
}

const DWORD mbpos3Rtn = 0x008645B9;
__declspec(naked) void mbpos3()
{
	__asm {
		push 186
		sub eax, edi
		push eax
		jmp mbpos3Rtn
	}
}