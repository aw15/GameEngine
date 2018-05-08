//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
    ::srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	BuildFrameBuffer(); 

	BuildObjects(); 

	return(true);
}

void CGameFramework::BuildFrameBuffer()
{
    HDC hDC = ::GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

    m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, (rcClient.right - rcClient.left) + 1, (rcClient.bottom - rcClient.top) + 1);
    ::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
    ::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
    HBRUSH hBrush = ::CreateSolidBrush(dwColor);
    HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, m_pPlayer->m_pCamera->m_Viewport.m_nWidth, m_pPlayer->m_pCamera->m_Viewport.m_nHeight);
	/*std::cout << m_pPlayer->m_pCamera->m_Viewport.m_yStart << " " << m_pPlayer->m_pCamera->m_Viewport.m_nWidth << std::endl;*/
    ::SelectObject(m_hDCFrameBuffer, hOldBrush);
    ::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer()
{    
    HDC hDC = ::GetDC(m_hWnd);
    ::BitBlt(hDC, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, m_pPlayer->m_pCamera->m_Viewport.m_nWidth, m_pPlayer->m_pCamera->m_Viewport.m_nHeight, m_hDCFrameBuffer, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, SRCCOPY);
    ::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);

			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			::ReleaseCapture();
			break;
		case WM_MOUSEMOVE:
			break;
		default:
			break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
					::PostQuitMessage(0);
					break;
				case VK_RETURN:
					break;
				default:
					m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
					break;
			}
			break;
		default:
			break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
				m_GameTimer.Stop();
			else
				m_GameTimer.Start();
			break;
		}
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
		}
		return(0);
}

void CGameFramework::BuildObjects()
{
	CAirplaneMesh *pAirplaneMesh = new CAirplaneMesh(6.0f, 6.0f, 1.0f);
	pAirplaneMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(3.0f, 3.0f, 30.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	m_pPlayer = new CPlayer();
	m_pPlayer->SetPosition(0.0f, 0.0f, -500.0f);
	m_pPlayer->SetMesh(pAirplaneMesh);
	m_pPlayer->SetColor(RGB(0, 0, 255));
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
	
	CBossMesh *pBossMesh = new CBossMesh(40.0f, 20.0f, 40.0f);
	pBossMesh->SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(20.0f, 20.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	m_pBoss = new CBoss();
	m_pBoss->SetPosition(10.0f, 0.0f,500.0f);
	m_pBoss->Rotate(0, 180, 0 );
	m_pBoss->SetMesh(pBossMesh);
	m_pBoss->SetColor(RGB(255, 100, 100));
	m_pBoss->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));

	m_pScene = new CScene();
	m_pScene->BuildObjects();

	m_pScene->m_pPlayer = m_pPlayer;
	m_pScene->m_pBoss = m_pBoss;
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}

	if (m_pPlayer) delete m_pPlayer;
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
    if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);

    if (m_hWnd) DestroyWindow(m_hWnd);
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	if (GetKeyboardState(pKeyBuffer))
	{
		
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
	}
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	if (GetCapture() == m_hWnd)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 5.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 5.0f;
		/*std::cout << m_Viewport.m_nHeight << std::endl;*/
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		float xValue=((2.0f*(ptCursorPos.x-700)) / m_pPlayer->m_pCamera->m_Viewport.m_nWidth) - 1;
		float yValue = (-(2.0f*ptCursorPos.y) / m_pPlayer->m_pCamera->m_Viewport.m_nHeight) + 1;
		float ratio = m_pPlayer->m_pCamera->m_Viewport.m_nWidth / m_pPlayer->m_pCamera->m_Viewport.m_nHeight;
		float yScale = tan(60.0f / 2);
		yScale = 1 / yScale;
		float xScale = yScale / ratio;
		m_pScene->SetRay( { xValue / xScale, yValue / yScale,1 });
	}
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		if (dwDirection)
		{
			if (m_pPlayer->GetPosition().y > 20)
			{
				m_pPlayer->m_xmf3Position.y -= 1;
				m_pPlayer->SetColor(RGB(250, 50, 250));
			}
			else if (m_pPlayer->GetPosition().y < -20)
			{
				m_pPlayer->m_xmf3Position.y += 1;
				m_pPlayer->SetColor(RGB(250, 50, 250));
			}
			else if (m_pPlayer->GetPosition().x > 30)
			{
				m_pPlayer->m_xmf3Position.x -= 1;
				m_pPlayer->SetColor(RGB(250, 50, 250));
			}
			else if (m_pPlayer->GetPosition().x < -30)
			{
				m_pPlayer->m_xmf3Position.x += 1;
				m_pPlayer->SetColor(RGB(250, 50, 250));
			}
			else if (m_pPlayer->GetPosition().z < -500)
			{
				m_pPlayer->m_xmf3Position.z += 1;
				m_pPlayer->SetColor(RGB(250, 250, 0));
			}
			else if (m_pPlayer->GetPosition().z > 400)
			{
			}
			else
			{
				m_pPlayer->Move(dwDirection, 1.3f);
				m_pPlayer->SetColor(RGB(0, 0, 255));
			}
		}
	}
	
}

void CGameFramework::FrameAdvance()
{    
    if (!m_bActive) return;

	m_GameTimer.Tick(0.0f);


	if (m_pBoss->m_hp < 0 || m_pPlayer->m_hp < 0)
	{
		m_pScene->Restart();
	}

	static float totalTime = 0;
	totalTime += m_GameTimer.GetTimeElapsed();
	if (totalTime > 1)
	{
		XMFLOAT3 dir;
		dir.x = m_pPlayer->m_xmf3Position.x - m_pBoss->m_xmf3Position.x;
		dir.y = m_pPlayer->m_xmf3Position.y - m_pBoss->m_xmf3Position.y;
		dir.z = m_pPlayer->m_xmf3Position.z - m_pBoss->m_xmf3Position.z;
		dir = Vector3::Normalize(dir);
		m_pBoss->Fire(50,dir);
		totalTime = 0;
	}

	ProcessInput();



	m_pScene->Animate(m_GameTimer.GetTimeElapsed());
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
	m_pBoss->Update(m_GameTimer.GetTimeElapsed());
	


	ClearFrameBuffer(RGB(255, 255, 255));

	m_pScene->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);
	m_pPlayer->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);
	m_pBoss->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);


	PresentFrameBuffer();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}


