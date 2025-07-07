#include "stdafx.h"
#include "codecaves.h"
#include "FixIme.h"
#include "FixBuddy.h"
#include "AddyLocations.h"
#include "ReplacementString.h"

bool Client::WindowedMode = true;
bool Client::RemoveLogos = true;
int Client::setDamageCap = 199999;
int Client::setMAtkCap = 1999;
int Client::setAccCap = 999;
int Client::setAvdCap = 999;
double Client::setAtkOutCap = 199999;
int Client::speedMovementCap = 140;
bool Client::climbSpeedAuto = false;
float Client::climbSpeed = 1.0;
std::string Client::ServerIP = "127.0.0.1";
int Client::ServerPort = 8484;

void Client::UpdateGameStartup() {
	Memory::FillBytes(0x00C08459, 0x20, 0x00C0846E - 0x00C08459);//remove elevation requests
	Memory::WriteByte(0x00C08459, 0x22);//remove elevation requests	//thanks stelmo for showing me how to do this
	Memory::WriteString(0x00C08459 + 1, "asInvoker");//remove elevation requests	//not working from dll
	Memory::WriteByte(0x00C08463, 0x22);//remove elevation requests	//thanks stelmo for showing me how to do this
	Memory::WriteByte(0x0049C2CD + 1, 0x01);//remove elevation requests	//still not working unfortunately	//still keeping this to checks for admin privilege
	Memory::WriteByte(0x0049CFE8 + 1, 0x01);//likely requires affecting WINAPI CreateProcess, which requires a launcher		//because a packed client cannot be directly edited for these offsets
	Memory::WriteByte(0x0049D398 + 1, 0x01);//remove elevation requests	//still not working unfortunately

	Memory::FillBytes(0x00AFE084, 0x00, 0x006FE0B2 - 0x006FE084);//remove the existing server IP address in client
	const char* serverIP_Address = Client::ServerIP.c_str();
	Memory::WriteString(0x00AFE084, serverIP_Address);//write the user-set IP address
	Memory::WriteString(0x00AFE084 + 16, serverIP_Address);//write the user-set IP address
	Memory::WriteString(0x00AFE084 + 32, serverIP_Address);//write the user-set IP address
	Memory::WriteInt(0x007519C1 + 1, ServerPort);

	Memory::WriteInt(0x0077E055 + 1, 2147483646);
	Memory::WriteInt(0x0077E12F + 1, 2147483646);
	Memory::WriteInt(0x008C3304 + 1, setDamageCap);
	Memory::WriteInt(0x0077E215 + 1, setMAtkCap);
	Memory::WriteInt(0x00780620 + 1, setMAtkCap);
	Memory::WriteInt(0x007806D0 + 1, setAccCap);
	Memory::WriteInt(0x00780702 + 1, setAvdCap);
	Memory::WriteInt(0x0078FF5F + 1, 2147483646);
	Memory::WriteInt(0x0079166C + 1, 2147483646);
	Memory::WriteInt(0x00791CD5 + 1, 2147483646);
	Memory::WriteInt(0x0078E061 + 1, 2147483646);
	Memory::WriteInt(0x0078E67D + 1, 2147483646);
	Memory::WriteInt(0x007918FC + 1, 2147483646);

	Memory::WriteDouble(0x00AFE8A0, setAtkOutCap);

	Memory::WriteInt(0x00780743 + 3, speedMovementCap); //set speed cap //ty ronan
	Memory::WriteInt(0x008C4286 + 1, speedMovementCap); //set speed cap //ty ronan
	Memory::WriteInt(0x0094D91E + 1, speedMovementCap); //set speed cap //ty ronan
}

void Client::UpdateResolution() {
	if (WindowedMode) {
		unsigned char forced_window[] = { 0xb8, 0x00, 0x00, 0x00, 0x00 }; //force window mode	//thanks stelmo for showing me how to do this
		Memory::WriteByteArray(0x009F7A9B, forced_window, sizeof(forced_window));//force window mode
	}

	if (RemoveLogos) {
		Memory::PatchNop(0x0062EE54, 21);	//no Logo @launch //Thanks Denki!!
	}

	int height = 600;
	Memory::WriteInt(0x00522C73 + 1, height - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00522E65 + 1, height - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x0052307E + 1, height - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523359 + 1, height - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523595 + 1, height - 92); //various requests like party, guild, friend, family, invites that pop up //quest complete y axis
	Memory::WriteInt(0x0052378B + 1, height - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x0052397D + 1, height - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523BB1 + 1, height - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523DA5 + 1, height - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523FA3 + 1, height - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x005243DB + 1, height - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523154 + 1, height - 102); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x0052418C + 1, height - 102); //party quest available pop-up y axis my first address find own my own

	int reqPopOffset = 41;
	Memory::WriteInt(0x00523092 + 1, 464 - reqPopOffset); //various requests like party, guild, friend, family, invites that pop up //trade
	Memory::WriteInt(0x0052336D + 1, 464 - reqPopOffset); //various requests like party, guild, friend, family, invites that pop up //Party Invite
	Memory::WriteInt(0x00522E79 + 1, 464 - reqPopOffset); //various requests like party, guild, friend, family, invites that pop up //friend request
	Memory::WriteInt(0x00522C87 + 1, 464 - reqPopOffset); //various requests like party, guild, friend, family, invites that pop up //Guild Invite
	Memory::WriteInt(0x0052379F + 1, 464 - reqPopOffset); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523991 + 1, 464 - reqPopOffset); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523DC5 + 1, 464 - reqPopOffset); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523FB7 + 1, 464 - reqPopOffset); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x005243EF + 1, 464 - reqPopOffset); //various requests like party, guild, friend, family, invites that pop up
}

void Client::FixMouseWheel() {
	Memory::CodeCave(fixMouseWheelHook, 0x009E8090, 5);
}

void Client::Chinese() {
	FixIme::Hook();
	FixBuddy::Hook();

	Memory::WriteString(0x00AF2B28, ALLIANCE);
	Memory::WriteByte(0x008E55ED + 1, 0x0B);
	Memory::WriteByte(0x008E557A + 1, 0x0B);
	Memory::WriteByte(0x008E565E + 1, 0x0B);
	Memory::WriteByte(0x0090142E + 1, 0x5E);
	Memory::WriteByte(0x00901400 + 1, 1);

	// Fix Date Format
	Memory::CodeCave(fixDateFormat, 0x008EBF57, 14);
	Memory::CodeCave(fixDateFormat2, 0x008EBFA1, 14);
	Memory::CodeCave(fixDateFormat3, 0x008EC31A, 14);
	Memory::CodeCave(fixDateFormat4, 0x008EBF05, 14);

	// Fix Item Type
	Memory::CodeCave(getItemType1, 0x005CFA99, 15);
	Memory::CodeCave(getItemType2, getItemType2Addr, 27);

	// Fix Item Line Break
	Memory::PatchNop(0x008E4252, 2);

	// Fix Skill Line Break
	Memory::CodeCave(skillToolTip, 0x008F383E, 6);
}

DWORD Client::jumpCap = 123;
void Client::JumpCap() {
	Memory::CodeCave(customJumpCapHook1, 0x00780797, 10);
	Memory::CodeCave(customJumpCapHook2, 0x008C42A3, 10);
	Memory::CodeCave(customJumpCapHook3, 0x0094D942, 5);
	Memory::WriteInt(0x009CC6F9 + 2, 0x00C1CF80);
	if (climbSpeedAuto)
	{
		Memory::CodeCave(calcSpeedHook, 0x0094D93C, 6);
	}
	else {
		Memory::WriteDouble(0x00C1CF80, climbSpeed * 3.0);
	}
}

void Client::MoreHook() {
	//Super Tubi
	Memory::PatchNop(0x00485C01, 2);
	Memory::PatchNop(0x00485C21, 2);
	Memory::PatchNop(0x00485C32, 2);

	// Trump
	Memory::WriteInt(0x0045A5BE + 1, 9999);

	// Teleport on air
	Memory::PatchNop(0x00957C2D, 6);

	// Lacking level check removal
	Memory::WriteByte(0x008AD01A, 0xE9);
	Memory::WriteInt(0x008AD01A + 1, 0x008AD227 - (0x008AD01A + 5));

	// Make NX items droppable
	Memory::PatchNop(0x004F350C, 6);
	Memory::PatchNop(0x004F351E, 6);

	// Fix chat msg postion
	Memory::CodeCave(chatTextPos, 0x008DD06F, 6);

	// Fix guild emblem pixels
	Memory::WriteByte(0x005F12EF + 2, 0x05);

	// Fix monster book aligned text
	Memory::WriteByte(0x0086425B + 1, 82);
	Memory::CodeCave(mbpos1, 0x00864378, 5);
	Memory::CodeCave(mbpos2, 0x00864495, 5);
	Memory::CodeCave(mbpos3, 0x008645B4, 5);
	Memory::WriteInt(0x008646B8 + 1, 221);

	// Assaulter without target
	Memory::WriteByte(0x00951347 + 1, 0x1C);

	// Unlimited flash jump
	Memory::PatchNop(0x095071D, 2);
	Memory::PatchNop(0x096BF91, 6);
	Memory::PatchNop(0x096BF1B, 2);

	// Unlimited flash jump for all job
	Memory::PatchNop(0x0096C073, 6);
	Memory::SetHook(true, reinterpret_cast<void**>(&pDoActiveSkill), CUserLocal__DoActiveSkill_t);
	Memory::SetHook(true, reinterpret_cast<void**>(&pDoJump), CUserLocal_Jump);
	Memory::CodeCave((void*)FlashJumpAll, 0x0096BF0B, 0);
	
	//Disable pic check
	Memory::WriteByte(0x005F7CA1 + 1, 0x95);
	Memory::WriteByte(0x005F7CD4, 0x75);
	Memory::WriteByte(0x005F7CD4 + 1, 0x74);
	Memory::PatchNop(0x005F7DCD, 26);
	Memory::PatchNop(0x005F7E5F, 12);
}