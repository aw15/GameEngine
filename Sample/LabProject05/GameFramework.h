#pragma once

#define FRAME_BUFFER_WIDTH		640
#define FRAME_BUFFER_HEIGHT		480//������ũ��� ������ ���ϰ� �ߴ�. Ÿ���� ũ�⸦ �ٲٰ� �ߴ�.

#include "Timer.h"
#include "Scene.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void ChangeSwapChainState();

    void BuildObjects();
    void ReleaseObjects();

    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd; 

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;
        
	IDXGIFactory4				*m_pdxgiFactory;
	IDXGISwapChain3				*m_pdxgiSwapChain;
	ID3D12Device				*m_pd3dDevice;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;//����ü�� 2���� �ϰڴ�
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap;
	UINT						m_nRtvDescriptorIncrementSize;

	ID3D12Resource				*m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap;
	UINT						m_nDsvDescriptorIncrementSize;
	//������ ���� 2�� �ʿ�

	ID3D12CommandAllocator		*m_pd3dCommandAllocator;
	ID3D12CommandQueue			*m_pd3dCommandQueue;
	ID3D12GraphicsCommandList	*m_pd3dCommandList;

	ID3D12Fence					*m_pd3dFence;//CPU,GPU����ȭ�� ���� FEnce������Ʈ
	UINT64						m_nFenceValues[m_nSwapChainBuffers];//����ü�� ������ŭ ����ڴ�.
	HANDLE						m_hFenceEvent;

#if defined(_DEBUG)
	ID3D12Debug					*m_pd3dDebugController = NULL;
#endif

	D3D12_VIEWPORT				m_d3dViewport;
	D3D12_RECT					m_d3dScissorRect;

	CScene						*m_pScene;//�׷����ϴ� ���� ���⿡ �ִ�.

	CGameTimer					m_GameTimer;
	_TCHAR						m_pszFrameRate[50];
};
