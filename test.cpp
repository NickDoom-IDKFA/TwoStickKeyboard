#include <conio.h>
#include <iostream.h>
#include <fstream.h>
#include <windows.h>
#include <math.h>

#include "base_layout.h"
#include "Freq.h"

static const char VKeyNames[256][8]=
{"00h", "MouseL", "MouseR", "Break", "MouseM", "MouseX1", "MouseX2", "07h",
"Bksp", "Tab", "0Ah", "0Bh", "PadCntr", "Enter", "0Eh", "0Fh",
"Shift", "Ctrl", "Alt", "Pause", "CapsLk", "IME", "16h", "IMEJunj",
"IMEfinl", "IMEHK", "1Ah", "Esc", "IMECnv", "IMENCn", "IMEAcc", "IMEChn",
"Space", "PgUp", "PgDn", "End", "Home", "Left", "Up", "Right",
"Down", "Select", "Print", "Execute", "PrnSc", "Ins", "Del", "Help",
"0", "1", "2", "3", "4", "5", "6", "7",
"8", "9", "3Ah", "3Bh", "3Ch", "3Dh", "3Eh", "3Fh",
"40h", "A", "B", "C", "D", "E", "F", "G",
"H", "I", "J", "K", "L", "M", "N", "O",
"P", "Q", "R", "S", "T", "U", "V", "W",	//0x50..57
"X", "Y", "Z", "LWin", "RWin", "Menu", "5Eh", "Sleep",	//0x58..60
"Gray 0", "Gray 1", "Gray 2", "Gray 3", "Gray 4", "Gray 5", "Gray 6", "Gray 7",
"Gray 8", "Gray 9", "Gray *", "Gray +", "Gray ?", "Gray -", "Gray .", "Gray /",
"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8",
"F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16",
"F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24",
"88h", "89h", "8Ah", "8Bh", "8Ch", "8Dh", "8Eh", "8Fh",
"NumLk", "ScrlLk", "OEM92h", "OEM93h", "OEM94h", "OEM95h", "OEM96h", "97h",
"98h", "99h", "9Ah", "9Bh", "9Ch", "9Dh", "9Eh", "9Fh",
"LShift", "RShift", "LCtrl", "RCtrl", "LAlt", "RAlt", "URLBack", "URLFwrd",
"URLRefr", "URLStop", "URLSrch", "URLFav", "URLHome", "Mute", "VolDn", "VolUp",
"TrkNext", "TrkPrev", "TrkStop", "TrkPaus", "Mail", "Media", "App1", "App2",
"B8h", "B9h", ":;", "+", ",", "-", ".", "/?",
"`~", "C1h", "C2h", "C3h", "C4h", "C5h", "C6h", "C7h",
"C8h", "C9h", "CAh", "CBh", "CCh", "CDh", "CEh", "CFh",
"D0h", "D1h", "D2h", "D3h", "D4h", "D5h", "D6h", "D7h",
"D8h", "D9h", "DAh", "[{", "\|", "]}", "'\"", "OEMDFh",
"E0h", "OEME1h", "OEM <|\\", "OEME3h", "OEME4h", "IMEPrcs", "OEME6h", "Unicode",
"E8h", "OEME9h", "OEMEAh", "OEMEBh", "OEMECh", "OEMEDh", "OEMEEh", "OEMEFh",
"OEMF0h", "OEMF1h", "OEMF2h", "OEMF3h", "OEMF4h", "OEMF5h", "Attn", "CrSel",
"ExSel", "ErEOF", "Play", "Zoom", "FCh", "PA1", "Clear", "FFh" };

int FirstPressed,	//0 -- none yet, 1234 -- WASD, 5678 -- PL;'
	SecondPressed,	//0 -- none yet, 1234 -- WASD, 5678 -- PL;'
		FirstReleased;	//0 -- none yet, 1 -- WASD, 5 -- PL;'
int OperationMode = 0;	//0 -- testing layout, 1 -- trying to optimise layout;

int DeviceMode = 1;	//0 == Mobile (assuming sticks only), 1 == PC (assuming hardware Shift, Ctrl and Alt modifiers for fingers, sticks for thumbs).
int CapsLock = 0;	//0 == None, 1 == Once (only for Mobile mode), 2 == Lock (does not affect Numeric keys in PC mode).
int FnMode = 0;		//0 == None, 1 == Once, maybe also do 2 == Lock?
int HardwareShiftPressed = 0;

HWND hWndT;

static char Test_Diagram[256][512]={0};
void Test_DrawRect(int CenterX, int CenterY, int L, int D, int R, int U)
{
	int X, Y;
	for (X=CenterX+L; X<=CenterX+R; X++)
	 for (Y=CenterY+D; Y<=CenterY+U; Y+=U-D)
	  if (X>0&&X<512&&Y>0&&Y<256)Test_Diagram[Y][X]+=63;
	for (X=CenterX+L; X<=CenterX+R; X+=R-L)
	 for (Y=CenterY+D; Y<=CenterY+U; Y++)
	  if (X>0&&X<512&&Y>0&&Y<256)Test_Diagram[Y][X]+=63;
}

int DiagramStickX (int LR, int F, int S, int RL)	//������⭠� �ଠ ����ࠬ�� �᪫���� �⨪��. ������ "���襪" � �㪢���.
{
	int X=1;
	if (LR) X+=28;
	if (F==3) X+=16;
	else if (F==0 || F==2) X+=8;
	if (S==3) X+=8;
	else if (S==0 || S==2) X+=4;
	if (RL) X+=2;
	
	return X*2;
}
int DiagramStickY (int LR, int F, int S, int RL)	//LR ��� ����窨
{
	int Y=1;

	if (F==2) Y+=8;
	else if (F==1 || F==3) Y+=4;
	if (S==2) Y+=4;
	else if (S==1 || S==3) Y+=2;
	

	return Y*2;
}
int DiagramKeyX (int Key)	//����ࠬ�� �᪫���� ���筮� ���������� (������뢠���� ������ ����ࠬ�� �⨪��).
{
	int X=4;
	if (Key>11)
	{
		Key-=12;
		X++;
		if (Key>10)
		{
			Key-=11;
			X++;
		}
	}
	X+=Key*3;

	return X*2+1;
}
int DiagramKeyY (int Key)
{
	int Y=6;
	if (Key>11)
	{
		Key-=12;
		Y+=2;
		if (Key>10)
		{
			Key-=11;
			Y+=2;
		}
	}

	return Y*2;
}

char Russian[2][34]={"���������������������������������", "�������������������������������ި"};
int Spellcastings[33][4]={0};	//���⢥�����騥 ������ �㪢� (�����஢��� �� �� ��䠢���, � �� ��������������...) "�������� ���襡�묨 ����窠��" ��� �� �����.
//���� "���� ��������� �ࠢ�(1) ��� ����(0)".
//��ࢮ� �������� (0123=ULDR)
//��஥ �������� (� �� ᠬ��)
//����� �⨪ �� ���᪠�� ���� (�ࠢ�=1, ����=0).
//�� ����ࠬ�� ᫥�� �⮡ࠦ��� �����, ����ࠥ�� �� ���᪠��� �ࠢ��� �⨪� ����
//(��宫����᪨ � ����⨢�� ���� ������� "�ਬ����祭��" � ⮬� �⨪�, ����� �� ����������).
//���⮬� LR Flag (�� ���� �����) -> First -> Second -> RL Flag (�� ���� ���饭).

void InitNationalFont(char QwertyLayout[2][34])	//��� ���� ��⨬���樨 ������ �� �ਭ樯� "�ਢ�筮��", ������ ��㯯�஢��� � Qwerty.
{						//��� ��䠢�⮢ ����襣� ࠧ��� �ਤ���� ���-� ��ﬨ ��������. ����襣� -- �ᯮ�짮���� "������ S/RL", ��⮬� �� ���� �� "���������" ����� ���.
	int Key=0;
	for (int LR=0; LR<2; LR++)
	 for (int F=0; F<4; F++)
	  for (int S=0; S<4; S++)
	   for (int RL=0; RL<2; RL++)
	    if (KeyNames[LR][F][S][RL][0][0]=='A' && KeyNames[LR][F][S][RL][0][1]==0)	//"Alphabetic"
	    {
		KeyNames[LR][F][S][RL][0][0]=QwertyLayout[0][Key];
		KeyNames[LR][F][S][RL][1][0]=QwertyLayout[1][Key];
		KeyNames[LR][F][S][RL][1][1]=0;
		strcpy (KeyNames[LR][F][S][RL][2], "PA");	//a "magical" value for "permutable alphabetical"

		Test_DrawRect(DiagramStickX(LR,F,S,RL)*6 - 36,DiagramStickY(LR,F,S,RL)*6, -9,-12,9,12);
		Test_DrawRect(DiagramKeyX  (Key)      *6 - 36,DiagramKeyY  (Key)      *6, -15,-4,15,4);

		Spellcastings[Key][0]=LR;
		Spellcastings[Key][1]=F;
		Spellcastings[Key][2]=S;
		Spellcastings[Key][3]=RL;

		Key++;
	    }
	cout<<Key<<" of 33 used"<<endl;

	fstream Dia;
	Dia.open ("Test512x256x8.raw", ios::out|ios::binary);
	Dia.write (Test_Diagram[0], sizeof (Test_Diagram));
	Dia.close();
}

BOOL Status (int First, int Second, int Released)
{
	if (!FirstPressed)
	{
		return FALSE;
	} else if (!SecondPressed) {
		if (FirstPressed == First) return TRUE;
		return FALSE;
	} else if (!FirstReleased) {
		if (FirstPressed == First && SecondPressed == Second) return TRUE;
		return FALSE;
	}
	if (FirstPressed == First && SecondPressed == Second && FirstReleased == Released) return TRUE;
	return FALSE;
}

void DrawState()
{
	for (int F=1; F<=8; F++)
	{
		SendMessage(GetDlgItem(hWndT, F*100), BM_SETSTATE, Status(F, 0, 0), 0);
		for (int S=1; S<=8; S++)
		 for (int R=1; R<=5; R+=4) SendMessage(GetDlgItem(hWndT, F*100+S*10+R), BM_SETSTATE, Status(F, S, R), 0);
	}
}

void DrawNames()
{
	for (int F=0; F<8; F++)
	{
		SetDlgItemText(hWndT, (F+1)*100, KeyNames[F/4][F%4][4][2][0]);
		for (int S=0; S<8; S++)
		 for (int R=0; R<2; R++)
		 {
		 	if      (FnMode               && KeyNames[F/4][F%4][S%4][1-R][2][0] && strcmp (KeyNames[F/4][F%4][S%4][1-R][2], "PA"))	//����� Fn, �������� � ������ ���� Fn-���祭��, � �� ���祭�� �� ���� "�����᪮� ����⠭⮩", �⫨��饩 �㪢� �� �ᥣ� ��⠫쭮��
				SetDlgItemText(hWndT, (F+1)*100 + (S+1)*10 + 1+R*4, KeyNames[F/4][F%4][S%4][1-R][2]);
		 	else if (     (HardwareShiftPressed||CapsLock==1) && KeyNames[F/4][F%4][S%4][1-R][1][0] ||		//����� ������� ��� (����) ��� �ணࠬ��� (������), �������� � ������ ���� ⠪�� ���祭��.
		 		 CapsLock==2   &&   ( !strcmp(KeyNames[F/4][F%4][S%4][1-R][2],"PA") || !DeviceMode&&KeyNames[F/4][F%4][S%4][1-R][1][0] )    ) //��� ������ ०�� Caps Lock, � ������ ��� ��� �㪢�����, ��� � ��� ०�� ������ (� �� � ���孨� ॣ���� ���室�� ���, �� ����� ⠪����).
				SetDlgItemText(hWndT, (F+1)*100 + (S+1)*10 + 1+R*4, KeyNames[F/4][F%4][S%4][1-R][1]);
			else	SetDlgItemText(hWndT, (F+1)*100 + (S+1)*10 + 1+R*4, KeyNames[F/4][F%4][S%4][1-R][0]);
		 }
	}
}

float DistFactor=0;	//���樠�쭮, �⮡� � N/0 ���㫮 � ��砥 ����稢���.
float Distance (int Key)	//�����ﭨ� ����� ��������ﬨ �㪢� � ������� � �� ����ࠬ�� ����म�.
{
	float X=DiagramStickX(Spellcastings[Key][0],Spellcastings[Key][1],Spellcastings[Key][2],Spellcastings[Key][3]);
	X-=DiagramKeyX(Key);
	float Y=DiagramStickY(Spellcastings[Key][0],Spellcastings[Key][1],Spellcastings[Key][2],Spellcastings[Key][3]);
	Y-=DiagramKeyY(Key);

	return sqrt(X*X+Y*Y);
}

int CharsFactor=0;

float Difficulty (int Prev, int Key)	//��������� ������ �㪢� (� ���������), � ���⮬ ⮣�, �� ����� ���� ����� �।����.
{
	float Diff = Distance (Key) / DistFactor;		//�����ﭨ� �� �㪢� �� �� ���� � �ਢ�筮� ������� �業������� �� ����筮�� �ਭ樯�, � 業�� � ������� ����.
								//�� �������� �� ���� �᪫����, � ������ �㬬�ୠ� ᫮������ �������� ��ண� ࠢ��, ����� ����� �ਢ���� ���譥.
	//���砨, ����� ����� ��� (����ࠥ� � ���): -1 == ����� ��� �।��饣� (᫮�� ��砫��� � Key), ��� �� �।��饣� ᨬ���� ��⠫�� ������ �� �� �⨪, ����� �㦥�, ��� ��, �� ����� �� � �� ��஭�.
	if (Prev < 0 || Spellcastings[Key][0]!=Spellcastings[Prev][3] || Spellcastings[Key][1] != Spellcastings[Prev][1 + (Spellcastings[Prev][0]+Spellcastings[Prev][3])%2 ] )
	{
		if (Spellcastings[Key][0] == Spellcastings[Key][3]) //���� ������, ��ன ������ � �ࠧ� ����⨫�, ���� ����⨫� -- 3.5 ��������.
		{
//LRrl and RLlr costs 3.5 finger steps
			Diff += 3.5;
		} else {					    //���� ������, ��ன ������, ���� ����⨫�, ��ன ����⨫� -- 4 ��������.
//LRlr and RLrl costs 4 finger steps
			Diff += 4.0;
		}
	//���砨, ����� ���� ����� -- �� �।��饣� ᨬ���� ��⠫�� ������ ������ �� �⨪, ����� �㦥� ��� ᫥���饣�.
	} else {
		//Transition from LRrl/RLrl to LRrl costs 1.5 finger steps.
		//Transition from RLlr/LRlr to RLlr costs 1.5 finger steps.
		//Transition from LRrl/RLrl to LRlr costs 2 finger steps.
		//Transition from RLlr/LRlr to RLrl costs 2 finger steps.
		if (Spellcastings[Key][0] == Spellcastings[Key][3]) Diff+=1.5; else Diff+=2.0;
	}

	return Diff;
}


void HitVKey(int Key)
{
	char CurText[65536];

//	if (KeyEmu)
//	{		//����⠥� ��� �� �஢�� "�������� ॠ���㥬����", �� � �७ �� � ���. ��� �� ��� ������ �㪠�� ��� ����� �����.
		int CurLen;
		GetDlgItemText(hWndT, 1001, CurText, 65500);
		CurLen=strlen(CurText);
		GetDlgItemText(hWndT, Key, CurText+CurLen, 8);
		//ToDo here: process backspace, enter etc

		if (!strcmp(CurText+CurLen,"BkSp")) if (!CurLen) return; else CurLen-=2;

		if (!strcmp(CurText+CurLen,"Fn"))
		{
			FnMode = !FnMode;
			DrawNames();
		} else if (FnMode && Key)
		{
			FnMode=0;
			DrawNames();
		}

		if (!strcmp(CurText+CurLen,"Caps"))
		{
			if (DeviceMode) CapsLock = (!CapsLock)*2;
			else CapsLock = (CapsLock+1)%3;
			DrawNames();
		} else if (CapsLock==1 && Key)	//����ﭨ� "�������� ���� ࠧ" ���� ⮫쪮 � ०��� "�����쭠� ���������".
		{
			CapsLock=0;
			DrawNames();
		}

		if (!strcmp(CurText+CurLen,"Space")) CurText[CurLen]=' ';
		CurText[CurLen+1]=0;
		SetDlgItemText(hWndT, 1001, CurText);
//	} else {	//�� 㦥 ����� �㭪樮���쭠� ��������� -- ��� �� ॠ�쭮 � �����⨪� �㫨�㥬 ������ ������, ����� ����� ���-� �ᯮ�짮������.
//		GetDlgItemText(hWndT, Key, CurText, 8);
//VOID keybd_event(

  //  BYTE bVk,	// virtual-key code
    //BYTE bScan,	// hardware scan code
//    DWORD dwFlags,	// flags specifying various function options
  //  DWORD dwExtraInfo 	// additional data associated with keystroke
   //);	
		
//	}
}

int AutoRepeat;
int KeyEmu=1;


int ULDRl, ULDRr;

void ProcessKeys()
{
	if (KeyEmu)
	{
		ULDRl=0;
		if (GetAsyncKeyState(0x57)&0x8000) ULDRl=1;
		if (GetAsyncKeyState(0x41)&0x8000) ULDRl=2;
		if (GetAsyncKeyState(0x53)&0x8000) ULDRl=3;
		if (GetAsyncKeyState(0x44)&0x8000) ULDRl=4;
		ULDRr=0;
		if (GetAsyncKeyState(0x50)&0x8000) ULDRr=1;
		if (GetAsyncKeyState(0x4C)&0x8000) ULDRr=2;
		if (GetAsyncKeyState(0xBA)&0x8000) ULDRr=3;
		if (GetAsyncKeyState(0xDE)&0x8000) ULDRr=4;
	}
	if (DeviceMode)
	{
		int ShiftState;
		if (GetAsyncKeyState(VK_SHIFT)&0x8000) ShiftState=1;
		if (ShiftState != HardwareShiftPressed)
		{
			HardwareShiftPressed = ShiftState;
			DrawNames();
		}
	}
//cout<<ULDRl<<" "<<ULDRr<<endl;
	if (!FirstPressed && ULDRl) {FirstPressed=ULDRl; AutoRepeat=0;}
	else if (!FirstPressed && ULDRr) {FirstPressed=ULDRr+4; AutoRepeat=0;}
	else if (FirstPressed && !SecondPressed && FirstPressed<5 && ULDRr) SecondPressed=ULDRr+4;
	else if (FirstPressed>4 && !SecondPressed && ULDRl) SecondPressed=ULDRl;
	else if (FirstPressed && SecondPressed && !FirstReleased && !ULDRl) {FirstReleased=1; HitVKey(FirstPressed*100+SecondPressed*10+FirstReleased); AutoRepeat=0;}
	else if (FirstPressed && SecondPressed && !FirstReleased && !ULDRr) {FirstReleased=5; HitVKey(FirstPressed*100+SecondPressed*10+FirstReleased); AutoRepeat=0;}

	else if (FirstPressed && SecondPressed && FirstReleased == 1 && ULDRl) {if (SecondPressed>4) FirstPressed=SecondPressed; SecondPressed=ULDRl  ; FirstReleased=0;}	//fast re-combo
	else if (FirstPressed && SecondPressed && FirstReleased == 5 && ULDRr) {if (SecondPressed<5) FirstPressed=SecondPressed; SecondPressed=ULDRr+4; FirstReleased=0;}	//fast re-combo

	if (!ULDRl&&!ULDRr)
	{
		if (!SecondPressed) HitVKey(FirstPressed*100);
		FirstPressed=SecondPressed=FirstReleased=0;
	}

	if (FirstPressed && !SecondPressed && AutoRepeat>50) HitVKey(FirstPressed*100);
	if (FirstPressed && SecondPressed && FirstReleased && AutoRepeat>50) HitVKey(FirstPressed*100+SecondPressed*10+FirstReleased);
	AutoRepeat++;

//cout<<FirstPressed<<" "<<SecondPressed<<" "<<FirstReleased<<endl;
	DrawState();
}

float BestDifficulty = 1E30;
int Current[33][4];
void Permutate ()
{
	float Total;
	int Char, Prev;	//���ᬠ�ਢ����� ⥪��� �㪢� ��䠢�� � �।���� �㪢� (�।���� ����� ���� -1, �᫨ �� ��� � ᫮�� ��砫��� � ⥪�饩).
	int CastA[4], CastB[4], A, B;

	memcpy (Current, Spellcastings, sizeof (Current));

	for (int AtOnce = 0; AtOnce < 64; AtOnce++)
	{
		Total=0;

		if (BestDifficulty < 100500)	//��吝�, ��৪�� ���㭮�� 堪. ����: ���� ��� ���樠������� ᫮������ �᪫���� � �� ����砫쭮� ����, ��祣� �� ��������.
		{
		
			//Attempting to swap two random symbols;
//			int A = rand()*33/(RAND_MAX+1);
//			int B = rand()*33/(RAND_MAX+1);
//			while (A==B) B = rand()*33/(RAND_MAX+1);
	
			A = (rand() + timeGetTime())%33;
			B = (rand() + timeGetTime())%33;
	
			memcpy (CastA, Spellcastings[A], 4*sizeof(int));
			memcpy (CastB, Spellcastings[B], 4*sizeof(int));
			memcpy (Spellcastings[A], CastB, 4*sizeof(int));
			memcpy (Spellcastings[B], CastA, 4*sizeof(int));

		}
	
		//Calculating difficulty;
		for (Char=0; Char<33; Char++)
		{
			int Monogram = Single[Char];
			//��������� ��⮨� �� "��������" (�������) � "㧭������" (������ ���� �� ����筮�� �ਭ樯�, �᫨ �� �������� ��諮 ��� �� ���).
			//�⮡� ������, ��᪮�쪮 ���筠 ��������� ������ �㪢�, ��ᬮ�ਬ �� �� ��砨 �ਬ������: �� ��ਠ��� 33 ��㣨� �㪢 ��। ��� � � ����⢥ 34-�� -- ��ਠ��, �� � ��� ��砫��� ᫮��.
			for (Prev=0; Prev<33; Prev++)
			{
				Total += Difficulty (Prev, Char) * (float)(Pairs[Prev][Char]) / (float)CharsFactor;	//��������� ����� �㪢� ��᫥ ������ ��㣮� �㪢�, 㬭������� �� �⭮�⥫��� ���⭮��� ⠪�� ���ࠬ�.
				Monogram-=Pairs[Prev][Char];
			}
			if (Monogram<0)
			{
				cout<<"������� ����ᨫ��!"<<endl;
				cout<<"�ந��諠 ����������� �訡��, ����� �� ����� �ந�室��� �������: �㪢� � ���ࠬ��� ����砥��� � ����⢥ ��ன �㪢� ��, 祬 ��� ����� ����砥��� � �몥. ��� ������᪨� ⠡���� ���� 嫠�, ������ ����� ���⮢��� ���筨� ;)"<<endl;
				for(;;);
			}
			Total += Difficulty (-1, Char) * (float)Monogram / (float)CharsFactor;	//��⠢訥�� ��砨 �ਬ������ �㪢�, �� ��襤訥 � ���ࠬ�� -- ��� ࠧ ��砫쭠� �㪢� ᫮��.
		}

		if (Total<BestDifficulty)
		{
cout<<"Achieved "<<Total<<" at swap "<<AtOnce<<endl;		
			break;	//Success!!!
		}
	}

//cout<<A<<" "<<B<<" "<<Total<<" "<<BestDifficulty<<endl;
	if (Total>BestDifficulty)
	{
		//Revert swapped keys!
//		memcpy (Spellcastings[A], CastA, 4*sizeof(int));
//		memcpy (Spellcastings[B], CastB, 4*sizeof(int));

		memcpy (Spellcastings, Current, sizeof (Current));

	} else BestDifficulty = Total;
}

BOOL __export PASCAL TimerLoop (HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
        if (uMsg==WM_INITDIALOG)
        {
        	InitNationalFont(Russian);
//for (int i=0; i<1; i++) cout<<joySetCapture( hWnd, i, 1000, TRUE)<<" ";	//winmm.lib
//cout<<endl<<JOYERR_NOERROR<<" "<<MMSYSERR_NODRIVER<<" "<<JOYERR_NOCANDO<<" "<<JOYERR_UNPLUGGED<<endl;
        	hWndT=hWnd;
        	if (joySetCapture( hWnd, JOYSTICKID1, 10, TRUE)) cout<<"Joystick not found! Use WASD+PL;' while keeping console (not the main window!) in focus."<<endl;
        	else KeyEmu=0;
		DrawNames();

		for (int i=0;i<33;i++)
		{
			DistFactor=max(DistFactor,Distance(i));
//cout<<Russian[1][i]<<" <-> "<<Distance(i)<<endl;
		}
		DistFactor *= 1000;
        	SetTimer (hWnd, 0, 1, NULL);
        	return FALSE;
        }
        else if (uMsg==WM_CLOSE)
        {
		if (!KeyEmu) joyReleaseCapture(0);
                EndDialog (hWnd,NULL);
		return FALSE;
        }
        else if (uMsg==WM_TIMER)
        {
        	KillTimer (hWnd, 0);
        	if (OperationMode)
        	{
        		for (int i=0; i<16384; i++) Permutate();
        		cout<<"Achieved difficulty: "<<BestDifficulty<<endl;

			//Re-generate Alphanumeric keys: place them into KeyNames array and re-init GUI controls.
			for (int Key=0; Key<33; Key++)
			{
				KeyNames[ Spellcastings[Key][0] ] //���� "LR ���� �����"
					[ Spellcastings[Key][1] ] //���ࠢ����� ��ࢮ�� ��४��祭��
					[ Spellcastings[Key][2] ] //"����� �������" �������� ⮫쪮 ��५�� � ��直� ����-��ᯥ���, ⠪ �� ��� �ᥣ�� 0..3
					[ Spellcastings[Key][3] ] //���� "RL ���� ���饭", ���� ���� "�� ���᪠�� � ��������� ��⮯����" (����� �������) �� ������ � ��⨬���樨, ��� � ⥮ਨ �㪢� ��� 33-� ⠬ ����� ����.
								  //�� ���⮫쪮 ��㤮��� ᯮᮡ �����, �� (��)���⫨�� ������⥫� ��䠢�⮢ ������� ���᪮�� ����� �ࠧ� �뭥�� �㤠 ᠬ� ।��� �㪢� -- �� �� ᯠ��� ������� ����.
					[0] [0] = Russian[0][Key];	//���筠�, ���-�ନ���� 㦥 ���樠����஢�� �� ����.

				KeyNames[ Spellcastings[Key][0] ]
					[ Spellcastings[Key][1] ]
					[ Spellcastings[Key][2] ]
					[ Spellcastings[Key][3] ]
					[1] [0] = Russian[1][Key];	//���������, ���-�ନ���� 㦥 ���樠����஢�� �� ����.
			}
			fstream Save;
			Save.open ("Generated_Layout.hex", ios::out|ios::binary);
			Save.write ((char*)(Spellcastings[0]), sizeof (Spellcastings));
			Save.close();

			DrawNames();
        	}
		else ProcessKeys();
		SetTimer (hWnd, 0, 1, NULL); 
        }
        else if (uMsg==WM_COMMAND)
        {
                if (lParam)
                {
                        if (wParam>>16==BN_CLICKED)
                        {
				if ((wParam&0xFFFF)==1002)	//����祭�� ०��� ��⨬���樨 �᪫����
				{
					OperationMode = !OperationMode;
				}
				if ((wParam&0xFFFF)==1003)	//����㧪� ⥪�饩 �᪫���� ("��筨� ࠡ��� � ������ �⮩ ������")
				{
					fstream Load;
					Load.open ("Current_Layout.hex", ios::in|ios::binary);
					Load.read ((char*)(Spellcastings[0]), sizeof (Spellcastings));
					Load.close();

					for (int Key=0; Key<33; Key++)
					{
						KeyNames[ Spellcastings[Key][0] ] //���� "LR ���� �����"
							[ Spellcastings[Key][1] ] //���ࠢ����� ��ࢮ�� ��४��祭��
							[ Spellcastings[Key][2] ] //"����� �������" �������� ⮫쪮 ��५�� � ��直� ����-��ᯥ���, ⠪ �� ��� �ᥣ�� 0..3
							[ Spellcastings[Key][3] ] //���� "RL ���� ���饭", ���� ���� "�� ���᪠�� � ��������� ��⮯����" (����� �������) �� ������ � ��⨬���樨, ��� � ⥮ਨ �㪢� ��� 33-� ⠬ ����� ����.
										  //�� ���⮫쪮 ��㤮��� ᯮᮡ �����, �� (��)���⫨�� ������⥫� ��䠢�⮢ ������� ���᪮�� ����� �ࠧ� �뭥�� �㤠 ᠬ� ।��� �㪢� -- �� �� ᯠ��� ������� ����.
							[0] [0] = Russian[0][Key];	//���筠�, ���-�ନ���� 㦥 ���樠����஢�� �� ����.
		
						KeyNames[ Spellcastings[Key][0] ]
							[ Spellcastings[Key][1] ]
							[ Spellcastings[Key][2] ]
							[ Spellcastings[Key][3] ]
							[1] [0] = Russian[1][Key];	//���������, ���-�ନ���� 㦥 ���樠����஢�� �� ����.
					}
		
					DrawNames();
				}
				if ((wParam&0xFFFF)==1004)	//��४��祭�� � ०�� �������
				{
					DeviceMode = !DeviceMode;
					CapsLock = 0;
					FnMode = 0;
					HardwareShiftPressed = 0;
					DrawNames();
				}
			}
		}
        }
        else if (uMsg==MM_JOY1BUTTONDOWN || uMsg==MM_JOY1BUTTONUP)
        {
		if (wParam&JOY_BUTTON1) ULDRr=1;
		else if (wParam&JOY_BUTTON2) ULDRr=4;
		else if (wParam&JOY_BUTTON3) ULDRr=3;
		else if (wParam&JOY_BUTTON4) ULDRr=2;
		else ULDRr=0;
cout<<wParam<<" "<<LOWORD(lParam)<<" "<<HIWORD(lParam)<<endl;
        }
        else if (uMsg==MM_JOY1MOVE)
        {
		if (HIWORD(lParam)>45535) ULDRl=3;
		else if (LOWORD(lParam)>45535) ULDRl=4;
		else if (HIWORD(lParam)<20000) ULDRl=1;
		else if (LOWORD(lParam)<20000) ULDRl=2;
		else ULDRl=0;
cout<<wParam<<" "<<LOWORD(lParam)<<" "<<HIWORD(lParam)<<endl;
        }
        else return FALSE;
        return TRUE;
}

void main (void)
{/*
for(int A,b=0;b<30;b++)
{
	while(!kbhit()) A=rand();
//	cout<<A<<endl;
	A%=(10+26+26);
	if (A<10)
	{
		A+='0';
	} else if (A<10+26)
	{
		A+='A'-10;
	} else
	{
		A+='a'-10-26;
	}
	cout<<((char)(A));//<<endl;
	while(kbhit()) getch();
}	
*/	

	InitFreq();

/*	int i,j,Max,Cap=0x7FFFFFFF;	//������, ��᪨ ��吝� � ����� ᯮᮡ �஢����, �� ���ᨢ ���ࠬ� ��ᠫ��, �� �ண�� ᠬ ���ᨢ (�� ������ ���).
	while (Cap)
	{
		for (Max=i=0; i<33*33; i++)
		{
			if (Pairs[0][i] >= Cap) continue;	//Does not work if array contains equal values!
			if (Max<Pairs[0][i]) Max=Pairs[0][j=i];
		}
		cout<<Russian[1][j/33]<<Russian[1][j%33]<<" "<<Pairs[0][j]<<endl;
		Cap=Max;
	}
*/

	for (int Sec=0; Sec<33; Sec++)
	{
		int Count = Single[Sec];
		CharsFactor += Count;
		for (int Fir=0; Fir<33; Fir++) Count -= Pairs[Fir][Sec];
		cout<<Russian[1][Sec]<<": Total "<<Single[Sec]<<", "<<Count<<" word beginning"<<endl;
	}
	cout<<CharsFactor<<" chars total"<<endl;

	DialogBoxParam(GetModuleHandle(NULL),"TEST", NULL, TimerLoop, NULL);
}