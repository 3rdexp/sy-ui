/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * ά���Ŷӣ���������Ա���ˣ�QQȺ��79388177��
 */
#ifndef _CDirectUI_H
#define _CDirectUI_H

#ifdef WIN32
#pragma warning (disable: 4514 4786)
#endif

#if(WIN_VER >= 0x0500)
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define PANEL_DEVIDE_LINE 0xffb9b4ad

///////////////////////////////////////////////////////
//����DirectX��ؿ�
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
//#pragma comment(lib,"dxguid.lib")
#include "d3dx9.h"

#define CS_DROPSHADOW	0x20000
#define WS_EX_LAYERED	0x00080000
#define AC_SRC_ALPHA	0x01
#define ULW_ALPHA		0x02

#define LWA_COLORKEY	0x01
#define LWA_ALPHA		0x02

#define WM_MOUSEWHEEL	0x020A

//�Զ���ϵͳ������Ϣ
#define WM_TRAYICON_MSG (WM_USER + 100)

typedef struct 
{
	MOUSEHOOKSTRUCT		ms;
	DWORD				mouseData;
}MOUSEHOOKSTRUCTEX;

TRACKMOUSEEVENT csTME;

typedef BOOL (WINAPI* PRINTWINDOW)(HWND hWnd,HDC hdcBlt,UINT nFlags);  
typedef BOOL (WINAPI* SETLAYEREDWINDOWATTRIBUTES)(HWND,COLORREF,BYTE,DWORD);
typedef BOOL (WINAPI* UPDATELAYEREDWINDOWFUNCTION)(HWND,HDC,POINT*,SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD); 


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK EnumChildProc(HWND hwnd,LPARAM lParam);

void DoEvents();
void writeErrLog(char * msg, int lvl); 

static UPDATELAYEREDWINDOWFUNCTION	UpdateLayeredWindow;
static SETLAYEREDWINDOWATTRIBUTES	SetLayeredWindowAttributes;
static PRINTWINDOW					PrintWindow;

static LPDIRECT3DDEVICE9			g_pd3dDevice	= NULL;
static D3DPRESENT_PARAMETERS		d3dpp;

HMODULE						hUser32;
WNDCLASSEX					wndClass;
WNDCLASSEX					wc;
unsigned short			*	cursorID = IDC_ARROW;
bool						isStart = true;
#include <comdef.h>
#include <Exdisp.h>
#include <list>
using namespace std;

#include "com/IResource.h"				//��Դ����ӿ�
#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")
#include "com/CFont.h"					//�������ֿ�

CFont			*	g_font = NULL;

//#include "com/IResource.h"				//��Դ����ӿ�

//������
#include "com/CGraphics.h"				//��Ⱦ����
#include "com/CObject.h"				//����ԭ��
#include "com/CEvent.h"					//�¼�ԭ��

//�ӿڶ���ͷ�ļ�
//#include "com/IGraphics.h"				//��Ⱦ����ӿ�
#include "com/IObject.h"				//�������ӿ�
#include "com/IEvent.h"					//�¼�����ӿ�

//////////////////////////////////////////////////////
//�ؼ�������

//�����ؼ�
#include "controls/CPanel.h"			//�����ؼ�
#include "controls/CStage.h"			//������
#include "controls/CPicture.h"			//ͼ���
#include "controls/CButton.h"			//��ť
#include "controls/CScrollBar.h"		//������
#include "controls/CTileList.h"			//�����б��
#include "controls/CCheckBox.h"			//��ѡ��
#include "controls/CRadioBox.h"			//��ѡ��
#include "controls/CMenuCtrl.h"			//�˵�
#include "controls/CTextBox.h"			//�ı���
#include "controls/CListBox.h"			//�б��
#include "controls/CLabel.h"			//��ǩ��
#include "controls/CComboBox.h"			//��Ͽ�

#include "com/CXml.h"					//XML����  
 
//void onTextMenuChange(CEvent * evt);
bool event_EnumStageRefreshProc(CObject * child, void * param);

CMenuCtrl * textMenu = NULL;
//CMenuCtrl * chatLogMenu = NULL;

HWND	hMainWnd;
bool	isInit = false;
int		errorLevel = 1;

DWORD WINAPI drawThread(LPVOID lParam);


HRESULT InitDirect3D(HWND hwnd)
{
	LPDIRECT3D9				pD3D = NULL;
//	D3DDISPLAYMODE			d3ddm;

	if(NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION )))
		return E_FAIL;
	
// 	if( FAILED( pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT,&d3ddm)))
// 		return E_FAIL;	

	//�ѽṹ���㣬�����豸ʱ���ṹ��ԱֵΪ0��ʾʹ��ȱʡֵ
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	
	d3dpp.Windowed				= TRUE;								//ָ��������Direct3D��֧�ִ��ڵ��豸	
	d3dpp.SwapEffect			= D3DSWAPEFFECT_DISCARD;			//��̨������ʾ��ǰ̨�����Ժ�ԭ���ĺ�̨�����������Ϣ�Զ���ʧ
	d3dpp.BackBufferFormat		= D3DFMT_UNKNOWN;						//��̨�������ĵ��ʽ�뵱ǰ��ʾģʽ�ĵ��ʽһ��
	d3dpp.BackBufferWidth		= GetSystemMetrics(SM_CXSCREEN);	//��̨�������Ŀ��
	d3dpp.BackBufferHeight		= GetSystemMetrics(SM_CYSCREEN);	//��̨�������ĸ߶�
	d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;			//����Ƶ�ʣ�

	if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED,&d3dpp,&g_pd3dDevice)))
	{
		_cprintf("Direct3D CreateDevice with D3DCREATE_HARDWARE_VERTEXPROCESSING failed.\n");
		writeErrLog("Direct3D CreateDevice with D3DCREATE_HARDWARE_VERTEXPROCESSING failed.\n",1);
		//���D3DCREATE_HARDWARE_VERTEXPROCESSINGʧ�ܾ���D3DCREATE_SOFTWARE_VERTEXPROCESSING����
		if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED,&d3dpp,&g_pd3dDevice)))
		{
			_cprintf("Direct3D CreateDevice failed.\n");
			writeErrLog("Direct3D CreateDevice failed.\n",1);
			return E_FAIL;
		}
	}
	
	g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);	//�����ü�����
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);	//�ر�Z����
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	//���ñ�������
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);		//�رյƹ�
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,D3DFILL_SOLID);
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_SRCALPHA );
 	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA );
 	g_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, FALSE);
  	g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
 	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	//�ͷ�pD3D����
	SAFE_RELEASE(pD3D);

	return S_OK;
}


/////////////////////////////////////////////////////////////////
//DirectUI��ʼ������ 
void loadUI();
void initDirectUI(HINSTANCE hInst)
{
	#ifdef _DEBUG 
	AllocConsole();
	#endif 

	_cprintf("system start\n");

	//����͸����������API
	hUser32 = ::LoadLibrary(L"User32.dll");
	UpdateLayeredWindow			= (UPDATELAYEREDWINDOWFUNCTION)::GetProcAddress(hUser32,"UpdateLayeredWindow");
	SetLayeredWindowAttributes	= (SETLAYEREDWINDOWATTRIBUTES)::GetProcAddress(hUser32,"SetLayeredWindowAttributes");
	PrintWindow					= (PRINTWINDOW)::GetProcAddress(hUser32,"PrintWindow");

	FreeLibrary(hUser32);

	//ע�ᴰ����
	wc.hInstance		= hInst;
	wc.lpszClassName	= L"GDirectUIChildWindow";
	wc.lpfnWndProc		= ChildWndProc;
	wc.style			= NULL;
	wc.cbSize			= sizeof (WNDCLASSEX);
	wc.hIcon			= NULL;
	wc.hIconSm			= NULL;
	wc.hCursor			= NULL;
	wc.lpszMenuName		= NULL;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hbrBackground	= NULL;
	RegisterClassEx (&wc);

	//ע�ᴰ����
	wndClass.hInstance		= hInst;
	wndClass.lpszClassName	= L"GDirectUI";
	wndClass.lpfnWndProc	= WndProc;
	wndClass.style			=  CS_DBLCLKS| CS_HREDRAW | CS_VREDRAW;
	wndClass.cbSize			= sizeof (WNDCLASSEX);
	wndClass.hIcon			= NULL;
	wndClass.hIconSm		= NULL;//LoadIcon(wndClass.hInstance, (LPCTSTR)IDI_ICON);
	wndClass.hCursor		= LoadCursor (NULL, IDC_ARROW);
	wndClass.lpszMenuName	= NULL;
	wndClass.cbClsExtra	= 0;
	wndClass.cbWndExtra	= 0;
	wndClass.hbrBackground = NULL;//(HBRUSH)GetStockObject(NULL_BRUSH);
	RegisterClassEx (&wndClass);
	
	hMainWnd = CreateWindow (L"GDirectUIChildWindow",NULL,WS_POPUP,0,0,0,0,NULL,NULL,hInst,NULL);
	//���������߳�
	::CreateThread (NULL,0,drawThread,0,0,0);

	//�ȴ��������
	while(isInit==false);

	g_font = new CFont;
	
	/*
	//��ʼ���ı���˵�
	textMenu = new CMenuCtrl(0,10,10,160,200,0,L"TextMenu",false);
	textMenu->insert(L"����");
	textMenu->insert(L"����");
	textMenu->insert(L"ճ��");
	textMenu->insert(L"ɾ��");
	textMenu->insert(L"ȫѡ");
	textMenu->ResDisplay(60);
	textMenu->addEventListener(CEvent.CHANGE,(LPGUI_EVENT_FUNC)&onTextMenuChange);
	*/
	
	//��layout.xml�ļ��ж�ȡ�ؼ�λ��
	wstring  str = L"layout.xml";
	//str.append(L"\\layout.xml");

	GXML::getInstance()->LoadXML(str.c_str());
	GXML::getInstance()->GetNode();
}

void startDirectUI()
{
	while(isStart)
	{	
		DoEvents();
		iEvent->doEvent();
	}

	if(g_pd3dDevice !=NULL)
		g_pd3dDevice->Release();
}


void stopDirectUI()
{
	isStart = false;
	#ifdef _DEBUG 
	FreeConsole();
	#endif 
	ExitProcess(0);
}


inline void DoEvents()
{
	MSG msg;

	PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE);
	TranslateMessage(&msg);
	DispatchMessage(&msg);

// 	while (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE) ) 
// 	{
// 		TranslateMessage(&msg);
// 		DispatchMessage(&msg);
// 	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static DWORD cursor;
	static int	x,y,btn;
	static int  mx=0,my=0;
	POINT		pt;
	CStage * stage;
	static bMouseTrack = false;
	
	//��ȡ��ǰ�����stage
	iObject->eventStage = (CObject*)GetWindowLong(hwnd,GWL_USERDATA);

	if (iObject->eventStage == NULL)
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	switch (msg) 
	{
		case WM_ERASEBKGND:
			return true;

		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////
		//����¼� 

		//������
		case WM_LBUTTONDOWN:
			SetFocus(hwnd);//��������Ǵ���ʱ���ô��役��

			iObject->hFocusWnd = hwnd;
			
			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);
			ClientToScreen(hwnd,&pt);
			iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_DOWN,x,y,pt.x,pt.y,1,0);
			SetCapture(hwnd);
			break;

		case WM_LBUTTONUP:
			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);
			ClientToScreen(hwnd,&pt);
			iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_UP,x,y,pt.x,pt.y,1,0);
			ReleaseCapture();
			break;

		case WM_CAPTURECHANGED:
			if(iObject->eventStage->dragMode == true) 
				iObject->eventStage->stopDrag();
			break;

		case WM_LBUTTONDBLCLK:
			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);
			ClientToScreen(hwnd,&pt);
			iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_DBLCLICK,x,y,pt.x,pt.y,1,0);
			break;
			
		//����Ҽ�
		case WM_RBUTTONDOWN:
			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);
			ClientToScreen(hwnd,&pt);
			iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_DOWN,x,y,pt.x,pt.y,2,0);
			break;

		case WM_RBUTTONUP:
			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);
			ClientToScreen(hwnd,&pt);
			iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_UP,x,y,pt.x,pt.y,2,0);
			break;

		case WM_RBUTTONDBLCLK:
			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);
			ClientToScreen(hwnd,&pt);
			iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_DBLCLICK,x,y,pt.x,pt.y,2,0);
			break;
			
		//����м�
		case WM_MBUTTONDOWN:
			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);
			ClientToScreen(hwnd,&pt);
			iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_DOWN,x,y,pt.x,pt.y,3,0);
			break;

		case WM_MBUTTONUP:
			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);
			ClientToScreen(hwnd,&pt);
			iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_UP,x,y,pt.x,pt.y,3,0);
			break;

		case WM_MBUTTONDBLCLK:
			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);
			ClientToScreen(hwnd,&pt);
			iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_DBLCLICK,x,y,pt.x,pt.y,3,0);
			break;
			
		//����ƶ�
		case WM_MOUSEMOVE:
			if(x ==(int)(short)LOWORD(lParam) && y == (int)(short)HIWORD(lParam)) break;

			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);

			if(wParam & MK_LBUTTON) 
				btn = 1;
			else if(wParam & MK_RBUTTON)
				btn = 2;
			else if(wParam & MK_MBUTTON)
				btn = 3;
			else
				btn = 0;

			ClientToScreen(hwnd,&pt);

// 			if(iObject->eventStage->dragMode == true)
// 			{
// 				iObject->eventStage->move(pt.x - mx,pt.y - my);	
// 			}

			iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_MOVE,x,y,pt.x,pt.y,btn,0); 

			if (!bMouseTrack)
			{
				//mouse_hover�ṹ
				csTME.cbSize = sizeof (csTME);
				csTME.dwFlags = TME_LEAVE|TME_HOVER;
				csTME.hwndTrack = hwnd;
				csTME.dwHoverTime = 400;//SystemParametersInfo(SPI_GETMOUSEHOVERTIME);

				TrackMouseEvent (&csTME);
				bMouseTrack = true ;				
			}

			break;
			
		//�����ͣ
		case WM_MOUSEHOVER:

			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);

			if(wParam & MK_LBUTTON) 
				btn = 1;
			else if(wParam & MK_RBUTTON)
				btn = 2;
			else if(wParam & MK_MBUTTON)
				btn = 3;
			else
				btn = 0;

// 			if(btn == 0)
// 			{
				ClientToScreen(hwnd,&pt);
				iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_HOVER,x,y,pt.x,pt.y,btn,0);
				bMouseTrack = false;
			//}
			
			break;

		//����Ƴ�����
		case WM_MOUSELEAVE:
			bMouseTrack = false;
			iEvent->DispatchSystemEvent(CMouseEvent.MOUSE_OUT,iObject->eventStage,NULL);
			break;

		///������
		case WM_MOUSEWHEEL:

			int zDelta;
			zDelta = (short) HIWORD(wParam);    // wheel rotation
			pt.x = x = (int)(short)LOWORD(lParam);
			pt.y = y = (int)(short)HIWORD(lParam);

			if(LOWORD(wParam) & MK_LBUTTON) 
				btn = 1;
			else if(LOWORD(wParam) & MK_RBUTTON)
				btn = 2;
			else if(LOWORD(wParam) & MK_MBUTTON)
				btn = 3;
			else
				btn = 0;

			//��֪Ϊ�δ���Ϣ�����������������Ļ����(������Ϣ���ǿͻ�������)
			ScreenToClient(hwnd,&pt);
			iEvent->DispatchMouseEvent(CMouseEvent::MOUSE_WHEEL,pt.x,pt.y,x,y,btn,zDelta);
	 		break;


		//////////////////////////////////////////////////////////////////////////////
		//�����¼�

		case WM_KEYDOWN:
			iEvent->DispatchKeyboardEvent(CKeyboardEvent.KEY_DOWN,wParam);
			break;

		case WM_KEYUP:
			iEvent->DispatchKeyboardEvent(CKeyboardEvent.KEY_UP,wParam);
			break;


		//////////////////////////////////////////////////////////////////////////////
		//�����¼�

		case WM_WINDOWPOSCHANGED:
			break;

		case WM_SETCURSOR:
			SetCursor(LoadCursor(NULL,cursorID));
			return true;

		case WM_CHAR:
			iEvent->DispatchKeyboardEvent(CKeyboardEvent.KEY_CHAR,wParam);
			break;

		case WM_IME_CHAR:
			iEvent->DispatchKeyboardEvent(CKeyboardEvent.KEY_IME_CHAR,wParam);
			break;

		case WM_SETFOCUS:
		//	iEvent->DispatchSystemEvent(CEvent.FOCUS,(CObject*)iObject->eventStage,NULL);
		//	stage = (CStage *)iObject->eventStage;
	
		//	SetWindowPos(stage->hWnd_Shadow,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		//	SetWindowPos(stage->hWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
			EnumChildWindows(hwnd,&EnumChildProc,NULL);
			break;


		case WM_SIZE:
			stage = (CStage *)iObject->eventStage;
			SetWindowPos(stage->hWnd_Shadow,
				0,
				0,0,
				(int)(short)LOWORD(lParam)+stage->shadow_width*2,
				(int)(short)HIWORD(lParam)+stage->shadow_width*2,
				SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
			break;

		case WM_SHOWWINDOW:
			stage = (CStage *)iObject->eventStage;
			if(wParam)
				ShowWindow(stage->hWnd_Shadow,SW_SHOWNOACTIVATE);
			else
				ShowWindow(stage->hWnd_Shadow,SW_HIDE);
			break;

		case WM_KILLFOCUS:
			stage = (CStage *)iObject->eventStage;
			if(stage->doModel && stage->visible) 
			{
				SetForegroundWindow(stage->hWnd);
			}
			break;

		case WM_ACTIVATE:
			if(LOWORD(wParam) == WA_INACTIVE)
			{
				if(iObject->focusObject == iObject->eventStage)
				{
					iEvent->DispatchSystemEvent(CEvent.LOSTFOCUS,(CObject*)iObject->eventStage,NULL);
					iObject->focusObject = NULL;
				}
			}
			break;

		case WM_MOVE:
			iObject->eventStage->X = (int)(short) LOWORD(lParam);    // horizontal position 
			iObject->eventStage->Y = (int)(short) HIWORD(lParam);    // vertical position 
			stage = (CStage *)iObject->eventStage;
			SetWindowPos(stage->hWnd_Shadow,
				0,
				(int)(short) LOWORD(lParam)-stage->shadow_width,
				(int)(short) HIWORD(lParam)-stage->shadow_width,
				0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
			iEvent->DispatchSystemEvent(CEvent.MOVE,(CObject*)iObject->eventStage,NULL);
			DoEvents();
			iEvent->doEvent();
			break;

		case WM_PAINT:
			break;

		case WM_SYSCOMMAND:
			break;
		case WM_DESTROY:
			return true;

		case WM_CLOSE:
			iObject->eventStage->hide();
			return false;

	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK EnumChildProc(HWND hwnd,LPARAM lParam)
{
	if (TRUE == IsWindowVisible(hwnd))
	{
		SetWindowPos(hwnd,HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	}
	return TRUE;
}


LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_ERASEBKGND:
			return true;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}


//������ˢ��
bool event_EnumStageRefreshProc(CObject * child, void * param)
{
	if(child->stage == child)
	{
		((CStage*)child)->drawWindow();
	}
	return false;
}


/////////////////////////////////////////////////////////////////
//��������߳�
DWORD WINAPI drawThread(LPVOID lParam)
{
	//��ʼ��D3D
	InitDirect3D(hMainWnd);
	
	isInit = true;

	while(isStart)
	{	
		Sleep(40);
		
		//long s1 = timeGetTime();
		//���µ�ǰ����
		ObjectArray stages;
		ObjectArray::iterator it;
		
		iObject->getChildrenByType(OBJECT_STAGE,&stages);

		for(it=stages.begin();it!=stages.end();it++)
		{
			CStage * cs = (CStage*)(*it);

			if(cs->isUnload != true)
				cs->drawWindow();
		}

	//	iObject->enumChild(NULL,(ENUM_CHILD_FUNC*)&event_EnumStageRefreshProc,NULL);
		//_cprintf("time: %d \n",timeGetTime() - s1);
		//_cprintf("=============time:%d\n",timeGetTime() - s1);
	}
	
	return true;
}

void writeErrLog(char * msg, int lvl)
{
	if(lvl < errorLevel) return;
	
	char buff[1024];
	
	sprintf(buff,"%s\\inthebar\\errlog.txt",getenv("APPDATA"));
	
	FILE * fp = fopen(buff,"ab+");
	if(fp != NULL)
	{		
		time_t now; // �������� 
		time(&now); // ȡ�����ڵ�����ʱ�� 
		
		sprintf(buff,"date: %s\r\n%s\r\n----------------------------------------------------------------------\r\n",ctime(&now),msg);
		fwrite(buff,strlen(buff),1,fp);
		fclose(fp);
	} else {
		_cprintf("����errorLog�ļ�ʧ��!\n");
	}
}
#endif
