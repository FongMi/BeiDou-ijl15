#pragma once
class Client
{
public:
	static void UpdateGameStartup();
	static void UpdateResolution();
	static void FixMouseWheel();
	static void Chinese();
	static void JumpCap();
	static void MoreHook();
	static const int m_nIGCipherHash = 0XC65053F2;
	static int m_nGameHeight;
	static int m_nGameWidth;
	static bool WindowedMode;
	static bool RemoveLogos;
	static int setDamageCap;
	static int setMAtkCap;
	static int setAccCap;
	static int setAvdCap;
	static double setAtkOutCap;
	static bool climbSpeedAuto;
	static float climbSpeed;
	static int speedMovementCap;
	static DWORD jumpCap;
	static std::string ServerIP;
	static int ServerPort;
};