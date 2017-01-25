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

	return TRUE;
}
