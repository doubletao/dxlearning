#include "StdAfx.h"
#include "D3DXDeviceHelper.h"

CD3DXDeviceHelper & CD3DXDeviceHelper::Instance()
{
	static CD3DXDeviceHelper instance;
	return instance;
}

CD3DXDeviceHelper::CD3DXDeviceHelper(void)
	: _hInst(NULL)
	, _hWnd(NULL)
	, _nWidth(0)
	, _nHeight(0)
	, _bWindowed(FALSE)
	, _d3d9(NULL)
	, _vp(0)
	, _device(NULL)
	, bInitialized(FALSE)
{
	memset(&_cap9, 0, sizeof(D3DCAPS9));
	memset(&_d3dParam, 0, sizeof(D3DPRESENT_PARAMETERS));
}


CD3DXDeviceHelper::~CD3DXDeviceHelper(void)
{
}

void CD3DXDeviceHelper::SetBasicInfomation(HINSTANCE hInst, HWND hWnd, int nWidth, int nHeight, BOOL bWindowed)
{
	_hInst = hInst;
	_hWnd = hWnd;
	_nWidth = nWidth;
	_nHeight = nHeight;
	_bWindowed = bWindowed;
}

BOOL CD3DXDeviceHelper::InitialD3D()
{
	//创建接口指针
	_d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!_d3d9)
	{
		AfxMessageBox(_T("error when Direct3DCreate9"));
		return FALSE;
	}

	HRESULT hr = _d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &_cap9);
	if(D3D_OK != hr)
	{
		CString strMsg;
		strMsg.Format(_T("error when GetDeviceCaps,code=%d\n"), hr);
		//D3DERR_INVALIDCALL,D3DERR_INVALIDDEVICE,D3DERR_OUTOFVIDEOMEMORY,D3DERR_NOTAVAILABLE.
		return FALSE;
	}

	if (_cap9.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		_vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		_vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	_d3dParam.BackBufferWidth = _nWidth;
	_d3dParam.BackBufferHeight = _nHeight;
	_d3dParam.BackBufferFormat = D3DFMT_A8R8G8B8;
	_d3dParam.BackBufferCount = 1;
	_d3dParam.MultiSampleType = D3DMULTISAMPLE_NONE;
	_d3dParam.MultiSampleQuality = 0;
	_d3dParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
	_d3dParam.hDeviceWindow = _hWnd;
	_d3dParam.Windowed = _bWindowed;
	_d3dParam.EnableAutoDepthStencil = TRUE;
	_d3dParam.AutoDepthStencilFormat = D3DFMT_D24S8;
	_d3dParam.Flags = 0;
	_d3dParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	_d3dParam.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	hr = _d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _hWnd, _vp, &_d3dParam, &_device);
	if (D3D_OK != hr)
	{
		CString strMsg;
		strMsg.Format(_T("error when CreateDevice,code=%d\n"), hr);
		//D3DERR_DEVICELOST, D3DERR_INVALIDCALL, D3DERR_NOTAVAILABLE, D3DERR_OUTOFVIDEOMEMORY.
		return FALSE;
	}

	ASSERT(_device);

	bInitialized = TRUE;

	return TRUE;
}

IDirect3D9 * CD3DXDeviceHelper::GetD3D()
{
	return _d3d9;
}

IDirect3DDevice9 * CD3DXDeviceHelper::GetDevice()
{
	return _device;
}

BOOL CD3DXDeviceHelper::Initialized()
{
	return bInitialized;
}
