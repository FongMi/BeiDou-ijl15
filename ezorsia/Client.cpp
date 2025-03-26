#include "stdafx.h"
#include "AddyLocations.h"
#include "codecaves.h"
#include "FixIme.h"
#include "FixBuddy.h"

int Client::m_nGameWidth = 800;
int Client::m_nGameHeight = 600;
bool Client::WindowedMode = true;
bool Client::RemoveLogos = true;
int Client::setDamageCap = 199999;
int Client::setMAtkCap = 1999;
int Client::setAccCap = 999;
int Client::setAvdCap = 999;
double Client::setAtkOutCap = 199999;
bool Client::useTubi = true;
bool Client::SwitchChinese = true;
int Client::speedMovementCap = 140;
bool Client::climbSpeedAuto = false;
float Client::climbSpeed = 1.0;
unsigned char Client::imeType = 1;
std::string Client::ServerIP_AddressFromINI = "127.0.0.1";
int Client::serverIP_Port = 8484;
bool Client::talkRepeat = false;
int Client::talkTime = 2000;

void Client::UpdateGameStartup() {
	Memory::FillBytes(0x00C08459, 0x20, 0x00C0846E - 0x00C08459);//remove elevation requests
	Memory::WriteByte(0x00C08459, 0x22);//remove elevation requests	//thanks stelmo for showing me how to do this
	Memory::WriteString(0x00C08459 + 1, "asInvoker");//remove elevation requests	//not working from dll
	Memory::WriteByte(0x00C08463, 0x22);//remove elevation requests	//thanks stelmo for showing me how to do this
	Memory::WriteByte(0x0049C2CD + 1, 0x01);//remove elevation requests	//still not working unfortunately	//still keeping this to checks for admin privilege
	Memory::WriteByte(0x0049CFE8 + 1, 0x01);//likely requires affecting WINAPI CreateProcess, which requires a launcher		//because a packed client cannot be directly edited for these offsets
	Memory::WriteByte(0x0049D398 + 1, 0x01);//remove elevation requests	//still not working unfortunately

	Memory::FillBytes(0x00AFE084, 0x00, 0x006FE0B2 - 0x006FE084);//remove the existing server IP address in client
	const char* serverIP_Address = Client::ServerIP_AddressFromINI.c_str();
	Memory::WriteString(0x00AFE084, serverIP_Address);//write the user-set IP address
	Memory::WriteString(0x00AFE084 + 16, serverIP_Address);//write the user-set IP address
	Memory::WriteString(0x00AFE084 + 32, serverIP_Address);//write the user-set IP address
	Memory::WriteInt(0x007519C1 + 1, serverIP_Port);

	//optional non-resolution related stuff
	if (useTubi) { Memory::FillBytes(0x00485C32, 0x90, 2); }

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
	nStatusBarY = Client::m_nGameHeight - 578;

	Memory::CodeCave(AdjustStatusBar, dwStatusBarVPos, 5);
	Memory::CodeCave(AdjustStatusBarBG, dwStatusBarBackgroundVPos, 5);
	Memory::CodeCave(AdjustStatusBarInput, dwStatusBarInputVPos, 9);

	Memory::WriteInt(dwApplicationHeight + 1, m_nGameHeight);//push 600
	Memory::WriteInt(dwApplicationWidth + 1, m_nGameWidth);	//push 800 ; CWvsApp::InitializeGr2D
	Memory::WriteInt(dwCursorVectorVPos + 2, (unsigned int)floor(-m_nGameHeight / 2));//push -300				!!moves all interactable UI elements!!
	Memory::WriteInt(dwCursorVectorHPos + 2, (unsigned int)floor(-m_nGameWidth / 2));	//push -400 ; CInputSystem::SetCursorVectorPos				!!moves all interactable UI elements!!
	Memory::WriteInt(dwUpdateMouseLimitVPos + 1, m_nGameHeight);//mov ecx,600
	Memory::WriteInt(dwUpdateMouseLimitHPos + 1, m_nGameWidth);	//mov ecx,800 ; CInputSystem::UpdateMouse
	Memory::WriteInt(dwCursorPosLimitVPos + 1, m_nGameHeight);//mov eax,600
	Memory::WriteInt(dwCursorPosLimitHPos + 1, m_nGameWidth);	//mov eax,800 ; CInputSystem::SetCursorPos
	Memory::WriteInt(dwViewPortHeight + 3, m_nGameHeight);//lea eax,[esi+eax-600]
	Memory::WriteInt(dwViewPortWidth + 3, m_nGameWidth);	//lea eax,[ecx+eax-800]

	Memory::WriteInt(dwToolTipLimitVPos + 1, m_nGameHeight - 1); //mov eax,599 ; CUIToolTip::MakeLayer
	Memory::WriteInt(dwToolTipLimitHPos + 1, m_nGameWidth - 1); //mov eax,799 ; CUIToolTip::MakeLayer

	Memory::WriteInt(dwTempStatToolTipDraw + 3, -m_nGameWidth + 6); //lea eax,[eax+ecx-797] ; CTemporaryStatView::ShowToolTip
	Memory::WriteInt(dwTempStatToolTipFind + 3, -m_nGameWidth + 6); //lea eax,[eax+ecx-797] ; CTemporaryStatView::FindIcon
	Memory::WriteInt(dwTempStatIconVPos + 2, (m_nGameHeight / 2) - 23);	//sub ebx,277 ; Skill icon buff y-pos
	Memory::WriteInt(dwTempStatIconHpos + 3, (m_nGameWidth / 2) - 3);	//lea eax,[eax+esi+397] ; Skill icon buff x-pos
	Memory::WriteInt(dwTempStatCoolTimeVPos + 2, (m_nGameHeight / 2) - 23);	//sub ebx,277 ; Skill icon cooltime y-pos
	Memory::WriteInt(dwTempStatCoolTimeHPos + 3, (m_nGameWidth / 2) - 3);	//lea eax,[eax+esi+397] ; Skill icon cooltime x-pos

	Memory::WriteInt(dwByteAvatarMegaHPos + 1, m_nGameWidth); //push 800 ; CAvatarMegaphone::ByeAvatarMegaphone ; IWzVector2D::RelMove
	Memory::WriteInt(dwAvatarMegaWidth + 1, m_nGameWidth); //push 800 ; CAvatarMegaphone ; CreateWnd

	Memory::WriteInt(0x0043717B + 1, m_nGameHeight);//mov edi,600
	Memory::WriteInt(0x00437181 + 1, m_nGameWidth);	//mov esi,800 ; CreateWnd
	Memory::WriteInt(0x0053808B + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x00538091 + 1, m_nGameWidth);	//push 800 ; RelMove?
	Memory::WriteInt(0x004CC160 + 1, m_nGameWidth);	//mov [ebp-16],800 ; CreateWnd
	Memory::WriteInt(0x004CC2C5 + 2, m_nGameHeight);//cmp ecx,600
	Memory::WriteInt(0x004CC2B0 + 1, m_nGameWidth);	//mov eax,800 ; CreateWnd
	Memory::WriteInt(0x004D59B2 + 1, m_nGameHeight);//mov eax,800
	Memory::WriteInt(0x004D599D + 1, m_nGameWidth);	//mov eax,800 ; CreateWnd
	Memory::WriteInt(0x0085F36C + 2, m_nGameWidth);	//cmp edx,800
	Memory::WriteInt(0x0085F374 + 1, m_nGameWidth - 80);	//mov ecx,720 ; CreateDlg
	Memory::WriteInt(0x008EBC58 + 1, m_nGameHeight);//mov eax,600
	Memory::WriteInt(0x008EBC3C + 1, m_nGameWidth);	//mov eax,800 ; RelMove?
	Memory::WriteInt(0x009966B5 + 1, m_nGameHeight);//mov ecx,600
	Memory::WriteInt(0x009966CA + 2, m_nGameWidth);	//cmp edi,800
	Memory::WriteInt(0x009966D2 + 1, m_nGameWidth - 100);	//mov edx,700 ; CreateDlg
	Memory::WriteInt(0x009A3E7F + 1, m_nGameHeight);//mov edx,600
	Memory::WriteInt(0x009A3E72 + 1, m_nGameWidth);	//mov edx,800 ; CreateDlg
	Memory::WriteInt(0x0045B898 + 1, m_nGameWidth - 225);	//push 575 ##ED  //smega x axis fade
	Memory::WriteInt(0x005386F0 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x005386F5 + 1, m_nGameWidth);	//push 800 ; CField::DrawFearEffect
	Memory::WriteInt(0x0055B808 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x0055B80D + 1, m_nGameWidth);	//mov edi,800
	Memory::WriteInt(0x0055B884 + 1, m_nGameWidth);	//push 600 ; RelMove?
	Memory::WriteInt(0x007E15BE + 1, m_nGameWidth);	//push 800 ; CreateWnd
	Memory::WriteInt(0x007E16B9 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x007E16BE + 1, m_nGameWidth);	//push 800 ; CWnd::GetCanvas //!!length of server message at top
	Memory::WriteInt(0x008AA266 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x008AA26B + 1, m_nGameWidth);	//push 800 ; CreateWnd
	Memory::WriteInt(0x009F6E99 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x009F6EA0 + 1, m_nGameWidth);	//push 800 ; StringPool#1162 (MapleStoryClass)

	Memory::WriteInt(0x007CF48F + 1, m_nGameHeight);//mov eax,600 ; 
	Memory::WriteInt(0x007CF49D + 1, m_nGameWidth);	//mov eax,800 ; IWzVector2D::RelMove
	Memory::WriteInt(0x008A12F4 + 1, m_nGameHeight);//mov eax,600 ; 
	Memory::WriteInt(0x008A1302 + 1, m_nGameWidth);	//mov eax,800 ; IWzVector2D::RelMove
	Memory::WriteInt(0x007F257E + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x007F258F + 1, m_nGameWidth);	//push 800 ; CWnd::CreateWnd
	Memory::WriteInt(0x0046B85C + 1, m_nGameHeight);//mov eax,600
	Memory::WriteInt(0x0046B86A + 1, m_nGameWidth);	//mov eax,800 ; IWzVector2D::RelMove
	Memory::WriteInt(0x009994D8 + 1, m_nGameHeight);//mov ecx,600
	Memory::WriteInt(0x009994ED + 2, m_nGameWidth);	//cmp edi,800
	Memory::WriteInt(0x009994F5 + 1, m_nGameWidth - 100);	//mov edx,700 ; CreateDlg

	Memory::WriteInt(0x0062FC4A + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x0062FC4F + 1, m_nGameWidth);	//push 800 ; IWzGr2DLayer::Getcanvas
	Memory::WriteInt(0x0062FE63 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x0062FE68 + 1, m_nGameWidth);	//push 800 ; IWzGr2DLayer::Getcanvas
	Memory::WriteInt(0x0062F9C6 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x0062F9CB + 1, m_nGameWidth);	//push 800; (UI/Logo/Wizet)
	Memory::WriteInt(0x0062F104 + 1, m_nGameHeight);//mov eax,600
	Memory::WriteInt(0x0062F109 + 1, m_nGameWidth);	//mov eax,800 ; Rectangle
	Memory::WriteInt(0x006406D5 + 1, m_nGameHeight);//mov ecx,600
	Memory::WriteInt(0x006406C3 + 1, m_nGameWidth);	//mov ecx,800
	Memory::WriteInt(0x0064050A + 1, m_nGameHeight);//mov ecx,600
	Memory::WriteInt(0x006404F8 + 1, m_nGameWidth);	//mov ecx,800
	Memory::WriteInt(0x00640618 + 1, m_nGameHeight);//mov ecx,600
	Memory::WriteInt(0x00640690 + 1, m_nGameHeight);//mov ecx,600
	Memory::WriteInt(0x0064061D + 1, m_nGameHeight);//mov ecx,600
	Memory::WriteInt(0x0064064B + 1, m_nGameHeight);//mov ecx,600
	Memory::WriteInt(0x00640606 + 1, m_nGameWidth);	//mov ecx,800
	Memory::WriteInt(0x0064067E + 1, m_nGameWidth);	//mov ecx,800
	Memory::WriteInt(0x00640639 + 1, m_nGameWidth);	//mov ecx,800
	Memory::WriteInt(0x0064043E + 1, (unsigned int)floor(m_nGameWidth / 2));	//mov edi,400
	Memory::WriteInt(0x00640443 + 1, (unsigned int)floor(m_nGameHeight / 2));	//mov esi,300
	Memory::WriteInt(0x00640626 + 1, (unsigned int)floor(m_nGameWidth / 2));	//add eax,400 ; bunch of modulus stuff

	Memory::WriteInt(0x00641038 + 2, m_nGameHeight);//??possibly related to player display
	Memory::WriteInt(0x0064103F + 2, m_nGameWidth);//??possibly related to player display
	Memory::WriteInt(0x00641048 + 1, (unsigned int)floor(-m_nGameHeight / 2));	//mov esi,-300
	Memory::WriteInt(0x00641050 + 1, (unsigned int)floor(-m_nGameWidth / 2));		//mov esi,-400 ;
	Memory::WriteInt(0x00641A19 + 3, m_nGameHeight);//mov [ebp+28],600
	Memory::WriteInt(0x00641A12 + 3, m_nGameWidth);	//mov [ebp+32],800 ; idk
	Memory::WriteInt(0x00641B38 + 3, m_nGameHeight);//mov [ebp-32],600
	Memory::WriteInt(0x00641B2E + 3, m_nGameWidth);	//mov [ebp-36],800 ; CAnimationDisplayer::SetCenterOrigin

	Memory::WriteInt(0x006CD842 + 1, (unsigned int)floor(m_nGameWidth / 2));	//push 400 ; RelMove?

	Memory::WriteInt(0x0059A0A2 + 6, (unsigned int)floor(m_nGameHeight / 2));	//mov [ebx+2364],300
	Memory::WriteInt(0x0059A09C + 2, (unsigned int)floor(m_nGameWidth / 2));	//mov [esi],400	; CInputSystem::LoadCursorState
	Memory::WriteInt(0x0080546C + 1, m_nGameHeight);//mov edi,600
	Memory::WriteInt(0x00805459 + 1, m_nGameWidth);	//mov edx,800 ; CUIEventAlarm::CreateEventAlarm
	Memory::WriteInt(0x008CFD4B + 1, m_nGameHeight - 22);	//push 578
	Memory::WriteInt(0x008CFD50 + 1, m_nGameWidth);	//push 800
	Memory::WriteInt(0x0053836D + 1, (unsigned int)floor(-m_nGameHeight / 2));//push -300
	Memory::WriteInt(0x00538373 + 1, (unsigned int)floor(-m_nGameWidth / 2));	//push -400	; RelMove?
	Memory::WriteInt(0x0055BB2F + 1, (unsigned int)floor(-m_nGameHeight / 2));//push -300
	Memory::WriteInt(0x0055BB35 + 1, (unsigned int)floor(-m_nGameWidth / 2));	//push -400 ; RelMove?

	Memory::WriteInt(0x005A8B46 + 1, m_nGameHeight);//mov eax,600
	Memory::WriteInt(0x005A8B56 + 1, m_nGameWidth);	//mov eax,800 ; RelMove?
	Memory::WriteInt(0x005A9B42 + 1, m_nGameHeight);//mov eax,600
	Memory::WriteInt(0x005A9B52 + 1, m_nGameWidth);	//mov eax,800 ; RelMove?
	Memory::WriteInt(0x005AADAA + 1, m_nGameHeight);//mov eax,600
	Memory::WriteInt(0x005AADBA + 1, m_nGameWidth);	//mov eax,800 ; RelMove?
	Memory::WriteInt(0x005ABC65 + 1, m_nGameHeight);//mov eax,600
	Memory::WriteInt(0x005ABC75 + 1, m_nGameWidth);	//mov eax,800 ; RelMove?
	Memory::WriteInt(0x005ACB29 + 1, m_nGameHeight);//mov eax,600
	Memory::WriteInt(0x005ACB39 + 1, m_nGameWidth);	//mov eax,800 ; RelMove?
	Memory::WriteInt(0x005C187E + 1, m_nGameHeight);//mov eax,600
	Memory::WriteInt(0x005C188E + 1, m_nGameWidth);	//mov eax,800 ; RelMove?
	Memory::WriteInt(0x005C2D62 + 1, m_nGameHeight);//mov eax,600
	Memory::WriteInt(0x005C2D72 + 1, m_nGameWidth);	//mov eax,800 ; RelMove?
	Memory::WriteInt(0x005E3FA0 + 1, m_nGameHeight);//push 600

	Memory::WriteInt(0x005F64DE + 1, (unsigned int)floor(-m_nGameHeight / 2));	//push -300 ;
	Memory::WriteInt(0x005F6627 + 1, (unsigned int)floor(-m_nGameHeight / 2));	//push -300 ;
	Memory::WriteInt(0x005F464D + 1, (unsigned int)floor(m_nGameWidth / 2));//??related to login game frame

	Memory::WriteInt(0x0060411C + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x0060F79B + 1, (m_nGameHeight / 2) - 201);//??possibly related to login utildlg
	Memory::WriteInt(0x0060F7A4 + 1, (m_nGameHeight / 2) - 181);//??possibly related to login utildlg
	Memory::WriteInt(0x0060F7AC + 1, (m_nGameWidth / 2) - 201);//??possibly related to login utildlg
	Memory::WriteInt(0x0060F7B5 + 1, (m_nGameWidth / 2) - 181);//??possibly related to login utildlg
	Memory::WriteInt(0x00613093 + 1, (m_nGameHeight / 2) - 200);//??likely related to login pop-up msg
	Memory::WriteInt(0x00613099 + 1, (m_nGameWidth / 2) - 45);//??likely related to login pop-up msg
	Memory::WriteInt(0x0061DAFF + 1, (m_nGameHeight / 2) - 150);//??likely related to login pop-up msg
	Memory::WriteInt(0x0061DB08 + 1, (m_nGameHeight / 2) - 130);//??likely related to login pop-up msg
	Memory::WriteInt(0x0061DB10 + 1, (m_nGameWidth / 2) - 201);//??likely related to login pop-up msg
	Memory::WriteInt(0x0061DB19 + 1, (m_nGameWidth / 2) - 181);//??likely related to login pop-up msg

	Memory::WriteInt(0x004372B1 + 1, (unsigned int)floor(-m_nGameHeight / 2));//push -300
	Memory::WriteInt(0x004372B6 + 1, (unsigned int)floor(-m_nGameWidth / 2));	//push -400 ; RelMove?
	Memory::WriteInt(0x006CE3AB + 1, m_nGameWidth);	//push 800
	Memory::WriteInt(0x007E1CB7 + 1, m_nGameWidth);	//push 800
	Memory::WriteInt(0x008D82F5 + 1, m_nGameHeight - 22);	//push 578
	Memory::WriteInt(0x008D82FA + 1, m_nGameWidth);	//push 800 ; CreateWnd?
	Memory::WriteInt(0x00935870 + 1, (unsigned int)floor(m_nGameHeight / 2));	//push 300
	Memory::WriteInt(0x0093586B + 1, m_nGameWidth);	// push 800 ; RelMove? (Skills)
	Memory::WriteInt(0x009DFD5C + 1, m_nGameWidth);	//mov ecx,800
	Memory::WriteInt(0x009DFED2 + 1, m_nGameHeight);//mov ecx,600	; IWzVector2D::RelMove
	Memory::WriteInt(0x009F6ADD + 1, (unsigned int)floor(m_nGameHeight / 2)); //push 300 ; MapleStoryClass
	Memory::WriteInt(0x006D50D8 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x0074BAA9 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x0074B951 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x0074B4A2 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x0074B3B7 + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x006421B3 + 1, m_nGameHeight);//push 600 ; CSoundMan::PlayBGM

	Memory::WriteInt(0x0059EB49 + 1, m_nGameHeight);//push 600 ; CSoundMan::PlayBGM
	Memory::WriteInt(0x008D247B + 1, m_nGameHeight - 33);	//push 567 ; IWzVector2D::RelMove
	Memory::WriteInt(0x008DEB93 + 1, m_nGameHeight - 20);	//push 580
	Memory::WriteInt(0x008DEE2F + 1, m_nGameHeight - 20);	//push 580
	Memory::WriteInt(0x008D2765 + 1, m_nGameHeight - 19);	//push 581
	Memory::WriteInt(0x008DA11C + 1, m_nGameHeight - 19);	//push 581
	Memory::WriteInt(0x008D29B4 + 1, m_nGameHeight - 19);	//push 581
	Memory::WriteInt(0x008D8BFE + 1, m_nGameHeight - 19);	//push 581
	Memory::WriteInt(0x008D937E + 1, m_nGameHeight - 19);	//push 581 //008D9373  move mana bar outline? //ty rynyan
	Memory::WriteInt(0x008D9AC9 + 1, m_nGameHeight - 19);	//push  
	Memory::WriteInt(0x008D1D50 + 1, m_nGameHeight - 22);	//push 578
	Memory::WriteInt(0x008D1D55 + 1, m_nGameWidth);	//push 800
	Memory::WriteInt(0x008D1FF4 + 1, m_nGameHeight - 22);	//push 578
	Memory::WriteInt(0x008D1FF9 + 1, m_nGameWidth);	//push 800 ; CUIStatusBar
	Memory::WriteInt(0x0062F5DF + 1, m_nGameHeight);//push 600
	Memory::WriteInt(0x0062F5E4 + 1, m_nGameWidth);	//push 800 ; (UI/Logo/Nexon)
	Memory::WriteInt(0x004EDB89 + 1, m_nGameWidth);	//mov ecx,800
	Memory::WriteInt(0x004EDB78 + 1, m_nGameHeight);//mov ecx,600 ; CreateWnd
	Memory::WriteInt(0x004EDAD8 + 1, m_nGameWidth);	//mov ecx,800
	Memory::WriteInt(0x009F7079, m_nGameHeight);	// dd 600
	Memory::WriteInt(0x009F707E, m_nGameWidth);	// dd 800
	Memory::WriteInt(0x00BE2738, (unsigned int)floor(m_nGameWidth / 2));	// dd 400
	Memory::WriteInt(0x00BE2DF4, (unsigned int)floor(m_nGameHeight / 2));	// dd 300
	Memory::WriteInt(0x00BE2DF0, (unsigned int)floor(m_nGameWidth / 2));	// dd 400
	Memory::WriteInt(0x00640656 + 2, (unsigned int)floor(-m_nGameWidth / 2));		//add edi,-400 ;

	Memory::WriteInt(0x006CE4C6 + 1, (unsigned int)floor(-m_nGameWidth / 2));		//push -400 ;
	Memory::WriteInt(0x009E2E85 + 1, (unsigned int)floor(-m_nGameHeight / 2));	//push -300		overall screen visible UI scaling
	Memory::WriteInt(0x009E2E8B + 1, (unsigned int)floor(-m_nGameWidth / 2));		//push -400 ;	overall screen visible UI scaling

	Memory::WriteInt(0x0093519A + 1, (unsigned int)floor(-m_nGameHeight / 2));	//push -300 ;
	Memory::WriteInt(0x00954433 + 1, (unsigned int)floor(-m_nGameHeight / 2));	//push -300 ;
	Memory::WriteInt(0x00981555 + 1, (unsigned int)floor(-m_nGameHeight / 2));	//push -300 ;
	Memory::WriteInt(0x00981F7A + 2, (unsigned int)floor(-m_nGameHeight / 2));	//push -300 ;

	Memory::WriteInt(0x0066BACE + 2, (unsigned int)floor(-m_nGameWidth / 2));		//and ecx,-400
	Memory::WriteInt(0x009B76BD + 3, (unsigned int)floor(-m_nGameHeight / 2));	//push -300
	Memory::WriteInt(0x009B76CB + 3, (unsigned int)floor(m_nGameHeight / 2));		//push 300

	Memory::WriteInt(0x009F7078 + 1, m_nGameHeight);//??related to application dimensions	//(ragezone release merge)//thanks mr mr of ragezone for these addresses
	Memory::WriteInt(0x009F707D + 1, m_nGameWidth);//??related to application dimensions

	Memory::WriteInt(0x0058C8A6 + 1, m_nGameWidth);//??

	Memory::WriteInt(0x004EDABF + 1, m_nGameHeight);//??

	Memory::WriteInt(0x00991854 + 1, m_nGameHeight);//??unknown cwnd function
	Memory::WriteInt(0x0099185F + 1, (m_nGameWidth / 2) - 134);//??unknown cwnd function
	Memory::WriteInt(0x00991867 + 1, (m_nGameWidth / 2) - 133);//??unknown cwnd function
	Memory::WriteInt(0x00992BA7 + 1, (unsigned int)floor(m_nGameWidth / 2));//??unknown cwnd function, possibly related to cutildlg
	Memory::WriteInt(0x00992BAC + 1, (unsigned int)floor(m_nGameHeight / 2));//??unknown cwnd function, possibly related to cutildlg

	Memory::WriteInt(0x007E1E07 + 2, m_nGameWidth);//??related to displaying server message at top of screen
	Memory::WriteInt(0x007E19CA + 2, m_nGameWidth);//??related to displaying server message at top of screen

	Memory::WriteInt(0x005362B2 + 1, (m_nGameWidth / 2) - 129);//??related to boss bar
	Memory::WriteInt(0x005364AA + 2, (m_nGameWidth / 2) - 128);//??related to boss bar

	Memory::WriteInt(0x00592A08 + 1, (m_nGameWidth / 2) - 125);//??likely related to mouse pos

	Memory::WriteInt(0x00621226 + 1, (m_nGameWidth / 2) - 216);//??possibly related to logo
	Memory::WriteByte(0x0062121E + 1, 0x01);//??possibly related to logo

	Memory::WriteInt(0x008C069F + 1, (m_nGameHeight / 2) - 14);//??related to status bar
	Memory::WriteInt(0x008C06A4 + 1, (m_nGameWidth / 2) - 158);//???related to status bar

	Memory::WriteInt(0x00A24D0B + 1, (m_nGameWidth / 2) - 129);//??

	Memory::WriteInt(0x00BE273C, 128);//??
	Memory::WriteByte(0x00A5FC2B, 0x05);//??
	Memory::WriteByte(0x00592A06 + 1, 0x01);//???likely related to mouse pos

	Memory::WriteInt(0x00744EB4 + 1, m_nGameWidth);//??related to in-game taking screenshot functionality
	Memory::WriteInt(0x00744EB9 + 1, m_nGameHeight);//??related to in-game taking screenshot functionality
	Memory::WriteInt(0x00744E2A + 1, 3 * m_nGameWidth * m_nGameHeight);//??related to in-game taking screenshot functionality
	Memory::WriteInt(0x00744E43 + 1, m_nGameWidth * m_nGameHeight);//??related to in-game taking screenshot functionality
	Memory::WriteInt(0x00744DA6 + 1, 4 * m_nGameWidth * m_nGameHeight);//??related to in-game taking screenshot functionality

	if (WindowedMode) {
		unsigned char forced_window[] = { 0xb8, 0x00, 0x00, 0x00, 0x00 }; //force window mode	//thanks stelmo for showing me how to do this
		Memory::WriteByteArray(0x009F7A9B, forced_window, sizeof(forced_window));//force window mode
	}
	if (RemoveLogos) {
		Memory::FillBytes(0x0062EE54, 0x90, 21);	//no Logo @launch //Thanks Denki!!
	}

	Memory::WriteInt(0x0045B337 + 1, m_nGameWidth);//related to smega display  //likely screen area where pop up starts for smega
	Memory::WriteInt(0x0045B417 + 1, m_nGameWidth - 225);//smega with avatar x axis for duration on screen

	Memory::WriteInt(0x00849E39 + 1, m_nGameHeight - 177); //system menu pop up
	Memory::WriteInt(0x0084A5B7 + 1, m_nGameHeight - 281); //shortcuts pop up	//0x84A5BD -  System Options "X" Position. if needed

	Memory::WriteInt(0x00522C73 + 1, m_nGameHeight - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00522E65 + 1, m_nGameHeight - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x0052307E + 1, m_nGameHeight - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523359 + 1, m_nGameHeight - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523595 + 1, m_nGameHeight - 92); //various requests like party, guild, friend, family, invites that pop up //quest complete y axis
	Memory::WriteInt(0x0052378B + 1, m_nGameHeight - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x0052397D + 1, m_nGameHeight - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523BB1 + 1, m_nGameHeight - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523DA5 + 1, m_nGameHeight - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523FA3 + 1, m_nGameHeight - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x005243DB + 1, m_nGameHeight - 92); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x00523154 + 1, m_nGameHeight - 102); //various requests like party, guild, friend, family, invites that pop up
	Memory::WriteInt(0x0052418C + 1, m_nGameHeight - 102); //party quest available pop-up y axis my first address find own my own

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

	Memory::WriteByte(0x00533B03, 0xb8); //boss bar extend to window
	Memory::WriteInt(0x00533B03 + 1, m_nGameWidth - 15); //boss bar	extend to window
	Memory::WriteByte(0x00534370, 0xb9); //boss bar	extend to window
	Memory::WriteInt(0x00534370 + 1, m_nGameWidth - 22); //boss bar	extend to window

	myHeight = (Client::m_nGameHeight - 600) / 2;//cash shop fix for frame area	//recalc offsets
	myWidth = (Client::m_nGameWidth - 800) / 2;//cash shop fix for frame area //recalc offsets

	nHeightOfsetted1 = 316; nWidthOfsetted1 = 256; nTopOfsetted1 = 0 + myHeight; nLeftOfsetted1 = 0 + myWidth; //parameters for fix1
	Memory::CodeCave(CashShopFix1, dwCashFix1, dwCashFix1NOPs);
	nHeightOfsetted2 = 104; nWidthOfsetted2 = 256; nTopOfsetted2 = 318 + myHeight; nLeftOfsetted2 = -1 + myWidth; //parameters for fix2
	Memory::CodeCave(CashShopFix2, dwCashFix2, dwCashFix2NOPs);
	nHeightOfsetted3 = 163; nWidthOfsetted3 = 246; nTopOfsetted3 = 426 + myHeight; nLeftOfsetted3 = 0 + myWidth; //parameters for fix3
	Memory::CodeCave(CashShopFix3, dwCashFix3, dwCashFix3NOPs);
	nHeightOfsetted4 = 78; nWidthOfsetted4 = 508; nTopOfsetted4 = 17 + myHeight; nLeftOfsetted4 = 272 + myWidth; //parameters for fix4
	Memory::CodeCave(CashShopFix4, dwCashFix4, dwCashFix4NOPs);
	nHeightOfsetted5 = 430; nWidthOfsetted5 = 412; nTopOfsetted5 = 95 + myHeight; nLeftOfsetted5 = 275 + myWidth; //parameters for fix5
	Memory::CodeCave(CashShopFix5, dwCashFix5, dwCashFix5NOPs);	//main part of shop, item listings	//thanks angel for stuff that helped
	nHeightOfsetted6 = 358; nWidthOfsetted6 = 90; nTopOfsetted6 = 157 + myHeight; //parameters for fix6
	Memory::CodeCave(CashShopFix6, dwCashFix6, dwCashFix6NOPs);//code cave 6 //best sellers
	Memory::WriteInt(0x004694BA + 1, myWidth + 690);//nleft, actual drawn part	//best sellers
	nHeightOfsetted7 = 56; nWidthOfsetted7 = 545; nTopOfsetted7 = 530 + myHeight; nLeftOfsetted7 = 254 + myWidth; //parameters for fix7
	Memory::CodeCave(CashShopFix7, dwCashFix7, dwCashFix7NOPs);
	nHeightOfsetted8 = 22; nWidthOfsetted8 = 89; nTopOfsetted8 = 97 + myHeight; nLeftOfsetted8 = 690 + myWidth; //parameters for fix8
	Memory::CodeCave(CashShopFix8, dwCashFix8, dwCashFix8NOPs);
	Memory::CodeCave(CashShopFixOnOff, dwCashFixOnOff, dwCashFixOnOffNOPs);	//fix for preview On/Off button not being accurate on entering cash shop //thanks windyboy

	nHeightOfsettedPrev = 165 + myHeight; nWidthOfsettedPrev = 212 + myWidth; nTopOfsettedPrev = 40 + myHeight; nLeftOfsettedPrev = 24 + myWidth; //parameters for fix cash preview
	Memory::CodeCave(CashShopFixPrev, dwCashFixPrev, dwCashFixPrevNOPs); //cash shop preview fix

	iHeightOfsetted1 = 200; iWidthOfsetted1 = 256; iTopOfsetted1 = 0 + myHeight; iLeftOfsetted1 = 0 + myWidth;
	Memory::CodeCave(ITCFix1, 0x0059E9E1, 12);
	iHeightOfsetted2 = 110; iWidthOfsetted2 = 256; iTopOfsetted2 = 200 + myHeight; iLeftOfsetted2 = -1 + myWidth;
	Memory::CodeCave(ITCFix2, 0x0059E9F9, 13);
	iHeightOfsetted3 = 108; iWidthOfsetted3 = 256; iTopOfsetted3 = 310 + myHeight; iLeftOfsetted3 = 0 + myWidth;
	Memory::CodeCave(ITCFix3, 0x0059EA15, 13);
	iHeightOfsetted4 = 180; iWidthOfsetted4 = 256; iTopOfsetted4 = 418 + myHeight; iLeftOfsetted4 = 0 + myWidth;
	Memory::CodeCave(ITCFix4, 0x0059EA31, 16);
	iHeightOfsetted5 = 78; iWidthOfsetted5 = 509; iTopOfsetted5 = 17 + myHeight; iLeftOfsetted5 = 272 + myWidth;
	Memory::CodeCave(ITCFix5, 0x0059EA4F, 16);	//main part of shop, item listings	//thanks angel for stuff that helped
	iHeightOfsetted6 = 48; iWidthOfsetted6 = 509; iTopOfsetted6 = 98 + myHeight; iLeftOfsetted6 = 273 + myWidth;
	Memory::CodeCave(ITCFix6, 0x0059EA6E, 14);//code cave 6 //best sellers
	iHeightOfsetted7 = 365; iWidthOfsetted7 = 509; iTopOfsetted7 = 145 + myHeight; iLeftOfsetted7 = 273 + myWidth;
	Memory::CodeCave(ITCFix7, 0x0059EA8B, 20);
	iHeightOfsetted8 = 56; iWidthOfsetted8 = 545; iTopOfsetted8 = 531 + myHeight; iLeftOfsetted8 = 255 + myWidth;
	Memory::CodeCave(ITCFix8, 0x0059EAAE, 17);

	Memory::WriteInt(0x00641F61 + 1, (unsigned int)floor(m_nGameWidth / 2));	//mov ebc,400 ;  VRleft		//camera movement
	Memory::WriteInt(0x00641FC8 + 1, (unsigned int)floor(m_nGameHeight / 2));	//add eax,300  ; VRTop //camera movement //not working for most maps
	Memory::WriteInt(0x0064208F + 1, (unsigned int)floor(m_nGameHeight / 2));	//add eax,300  ; VRbottom //camera movement //not working for most maps

	myAlwaysViewRestoreFixOffset = myHeight; //parameters for fix view restore all maps number ?????working????!!!
	Memory::CodeCave(AlwaysViewRestoreFix, dwAlwaysViewRestoreFix, dwAlwaysViewRestoreFixNOPs);	//fix view restora on all maps, currently does nothing; i likely looked in the wrong area

	nTopOfsettedVerFix = 10 + myHeight; nLeftOfsettedVerFix = 645 + myWidth; //parameters for fix version number
	Memory::CodeCave(VersionNumberFix, dwVersionNumberFix, dwVersionNumberFixNOPs);	//game version number fix //use this if you use no frame or default client frame

	nHeightOfsettedLoginBackCanvasFix = 352 + myHeight; nWidthOfsettedLoginBackCanvasFix = 125 + myWidth;//para for world select buttonsViewRec
	nTopOfsettedLoginBackCanvasFix = 125 + myHeight; nLeftOfsettedLoginBackCanvasFix = 0 + myWidth;
	Memory::CodeCave(ccLoginBackCanvasFix, dwLoginBackCanvasFix, LoginBackCanvasFixNOPs);	//world select buttons fix		//thank you teto for pointing out my error in finding the constructor

	nHeightOfsettedLoginViewRecFix = 167 + myHeight; nWidthOfsettedLoginViewRecFix = 540 + myWidth;//para for ViewRec fix
	nTopOfsettedLoginViewRecFix = 51 + myHeight; nLeftOfsettedLoginViewRecFix = 136 + myWidth;
	Memory::CodeCave(ccLoginViewRecFix, dwLoginViewRecFix, LoginViewRecFixNOPs);	//world ViewRec fix	

	a1x = 0 + myWidth; a2x = -149 + myWidth; a2y = 0 + myHeight; a3 = 25; a1y = -250; //a4 = 0;	//LoginDescriptor params
	Memory::WriteInt(0x0060D849 + 1, 300 + a1y); //speed 1	//temporary fix by increasing the speed of display until i get good enough at procedural programming 
	//and memory management and reverse engineering to use nexon's own functions to put a black layer with greater z value to cover the tabs being shown off screen at origin
	Memory::CodeCave(ccLoginDescriptorFix, dwLoginDescriptorFix, LoginDescriptorFixNOPs);	//world LoginDescriptor fix	

	int customEngY = -62, customEngX = -22, dojangYoffset = 0;	//myHeight //-55-35 (myHeight*250/100)	-(myWidth*53/100) 140 -130
	yOffsetOfMuruengraidPlayer = 50 + dojangYoffset; xOffsetOfMuruengraidPlayer = 169 + myWidth; //params
	Memory::CodeCave(ccMuruengraidPlayer, dwMuruengraidPlayer, MuruengraidPlayerNOPs);	//muruengraid scaling	
	yOffsetOfMuruengraidClock = 26 + dojangYoffset; xOffsetOfMuruengraidClock = 400 + myWidth; //params
	Memory::CodeCave(ccMuruengraidClock, dwMuruengraidClock, MuruengraidClockNOPs);	//muruengraid scaling
	yOffsetOfMuruengraidMonster = 50 + dojangYoffset; xOffsetOfMuruengraidMonster = 631 + myWidth; //params
	Memory::CodeCave(ccMuruengraidMonster, dwMuruengraidMonster, MuruengraidMonsterNOPs);	//muruengraid scaling
	yOffsetOfMuruengraidMonster1 = 32 + dojangYoffset; xOffsetOfMuruengraidMonster1 = 317 + myWidth; //params
	Memory::CodeCave(ccMuruengraidMonster1, dwMuruengraidMonster1, MuruengraidMonster1NOPs);	//muruengraid scaling	
	yOffsetOfMuruengraidMonster2 = 32 + dojangYoffset; xOffsetOfMuruengraidMonster2 = 482 + myWidth; //params
	Memory::CodeCave(ccMuruengraidMonster2, dwMuruengraidMonster2, MuruengraidMonster2NOPs);	//muruengraid scaling
	yOffsetOfMuruengraidEngBar = 86 + dojangYoffset + customEngY; xOffsetOfMuruengraidEngBar = 17 + myWidth + customEngX; //params
	Memory::CodeCave(ccMuruengraidEngBar, dwMuruengraidEngBar, MuruengraidEngBarNOPs);	//muruengraid scaling	
	yOffsetOfMuruengraidEngBar1 = 130 + dojangYoffset + customEngY; xOffsetOfMuruengraidEngBar1 = 20 + myWidth + customEngX; //params
	Memory::CodeCave(ccMuruengraidEngBar1, dwMuruengraidEngBar1, MuruengraidEngBar1NOPs);	//muruengraid scaling	
	yOffsetOfMuruengraidEngBar2 = 80 + dojangYoffset + customEngY; xOffsetOfMuruengraidEngBar2 = 9 + myWidth + customEngX; //params
	Memory::CodeCave(ccMuruengraidEngBar2, dwMuruengraidEngBar2, MuruengraidEngBar2NOPs);	//muruengraid scaling	
	yOffsetOfMuruengraidClearRoundUI = 260 + myHeight; xOffsetOfMuruengraidClearRoundUI = 400 + myWidth; //params
	Memory::CodeCave(ccMuruengraidClearRoundUI, dwMuruengraidClearRoundUI, MuruengraidClearRoundUINOPs);	//muruengraid scaling
	yOffsetOfMuruengraidTimerBar = 16 + dojangYoffset; xOffsetOfMuruengraidTimerBar = 345 + myWidth; //params
	Memory::CodeCave(ccMuruengraidTimerBar, dwMuruengraidTimerBar, MuruengraidTimerBarNOPs);	//muruengraid scaling
	xOffsetOfMuruengraidMonster1_2 = 318 + myWidth; //params	//finally fixed this bugger
	Memory::CodeCave(ccMuruengraidMonster1_2, dwMuruengraidMonster1_2, MuruengraidMonster1_2NOPs);	//muruengraid scaling

	darkCircleX = m_nGameWidth / 2 - 163;
	darkCircleY = m_nGameHeight / 2 - 190;
	Memory::CodeCave(darkMap1cc, 0x0055BEE6, 9);
	Memory::CodeCave(darkMap2cc, 0x0055C07F, 12);
	Memory::CodeCave(darkMap3cc, 0x0055C1C5, 13);
}

void Client::FixMouseWheel() {
	Memory::CodeCave(fixMouseWheelHook, 0x009E8090, 5);
}

void Client::Chinese() {
	if (Client::imeType == 0)
	{
		FixIme::HookOld();
	}
	else {
		FixIme::HookNew();
	}
	FixBuddy::Hook();
	if(SwitchChinese) {
		Memory::WriteString(0x00AF2B28, "¹ïÁp·ù     ");
		Memory::WriteByte(0x008E55ED + 1, 0x0B);
		Memory::WriteByte(0x008E557A + 1, 0x0B);
		Memory::WriteByte(0x008E565E + 1, 0x0B);
		Memory::WriteByte(0x0090142E + 1, 0x5E);
		Memory::WriteByte(0x00901400 + 1, 1);
	}
}

void Client::FixDateFormat() {
	if (SwitchChinese)
	{
		Memory::CodeCave(fixDateFormat, 0x008EBF57, 14); // StringPool 5273
		Memory::CodeCave(fixDateFormat2, 0x008EBFA1, 14); // StringPool 655
		Memory::CodeCave(fixDateFormat3, 0x008EC31A, 14); // StringPool 679
		Memory::CodeCave(fixDateFormat4, 0x008EBF05, 14); // StringPool 3138
	}
}

void Client::FixItemType() {
	if (SwitchChinese)
	{
		Memory::CodeCave(getItemType1, 0x005CFA99, 15);
		Memory::CodeCave(getItemType2, getItemType2Addr, 27);
	}
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

void Client::FixChatPosHook() {
	Memory::CodeCave(chatTextPos, 0x008DD06F, 6);
}

void Client::MoreHook() {
	Memory::WriteInt(0x009A3D81, 480);
	Memory::CodeCave(faceHairCave, 0x005C94F3, 18);
	Memory::CodeCave(canSendPkgTimeCave, 0x00485C28, 10);

	if (talkRepeat)
	{
		Memory::WriteByte(0x004905ED + 1, 5);
	}

	Memory::WriteInt(0x0049064B + 2, talkTime);

	if (setAtkOutCap > 999999)
	{
		Memory::WriteInt(0x008C485A + 1, 192);
		Memory::WriteInt(0x008C4AB3 + 1, 210);
		Memory::WriteInt(0x008C510A + 1, 218);
		Memory::WriteInt(0x008C4EA2 + 1, 210);
		Memory::WriteInt(0x008C5760 + 1, 210);
		Memory::WriteInt(0x008C7AD9 + 1, 185);
		Memory::WriteInt(0x008C2754 + 1, 195);
		Memory::WriteInt(0x008C6C72 + 1, 210);
		Memory::CodeCave(apDetailBtn, 0x008C4E1B, 7);
	}

	// trump
	Memory::WriteInt(0x0045A5BE + 1, 9999);

	// Unlimited Flash Jump
	Memory::PatchNop(0x095071D, 2);
	Memory::PatchNop(0x096BF91, 6);
	Memory::PatchNop(0x096BF1B, 2);

	// Teleport on air
	Memory::FillBytes(0x00957C2D, 0x90, 6);

	// Lacking Level Check Removal
	Memory::WriteByte(0x008AD01A, 0xE9);
	Memory::WriteInt(0x008AD01A + 1, 0x008AD227 - (0x008AD01A + 5));

	// Make NX items droppable
	Memory::FillBytes(0x004F350C, 0x90, 6);
	Memory::FillBytes(0x004F351E, 0x90, 6);
}

void Client::WorldMap()
{
	// WorldMap Cap Increase
	Memory::WriteByteArray(0x009EA030, world_cap_increase_array, sizeof(world_cap_increase_array));
	Memory::WriteInt(0x009EA030 + 2, 0xB4);
	wordMapX = (m_nGameWidth - 666) / 2;
	wordMapY = (m_nGameHeight - 524) / 2;
	Memory::CodeCave(wordMapUIcc, 0x009EB594, 13);
}