#include "StdAfx.h"
#include "D3DXDeviceHelper.h"

struct Vertex
{
	float _x;
	float _y;
	float _z;
	D3DCOLOR _color;
	Vertex(){}
	Vertex(float x, float y, float z, D3DCOLOR color)
		: _x(x)
		, _y(y)
		, _z(z)
		, _color(color)
	{
	}
	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

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
	, VB(NULL)
	, IB(NULL)
	, _x(0.0f)
	, _y(0.0f)
{
	memset(&_cap9, 0, sizeof(D3DCAPS9));
	memset(&_d3dParam, 0, sizeof(D3DPRESENT_PARAMETERS));
}


CD3DXDeviceHelper::~CD3DXDeviceHelper(void)
{
	Clear();
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

BOOL CD3DXDeviceHelper::Setup()
{
	_device->CreateVertexBuffer(8 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &VB, 0);
	_device->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &IB, 0);

	Vertex * vertices = NULL;
	VB->Lock(0, 0, (void **)&vertices, 0);
	vertices[0] = Vertex(-1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(255, 0, 0));
	vertices[1] = Vertex(-1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(255, 255, 0));
	vertices[2] = Vertex(1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(255, 255, 255));
	vertices[3] = Vertex(1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 255));
	vertices[4] = Vertex(-1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 255));
	vertices[5] = Vertex(-1.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0));
	vertices[6] = Vertex(1.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 0));
	vertices[7] = Vertex(1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 0));
	VB->Unlock();

	WORD * indics = NULL;
	IB->Lock(0, 0, (void **)&indics, 0);
	//font side
	indics[0] = 0;indics[1] = 1;indics[2] = 2;
	indics[3] = 0;indics[4] = 2;indics[5] = 3;
	//back side
	indics[6] = 4;indics[7] = 6;indics[8] = 5;
	indics[9] = 4;indics[10] = 7;indics[11] = 6;
	//left side
	indics[12] = 4;indics[13] = 5;indics[14] = 1;
	indics[15] = 4;indics[16] = 1;indics[17] = 0;
	//right side
	indics[18] = 3;indics[19] = 2;indics[20] = 6;
	indics[21] = 3;indics[22] = 6;indics[23] = 7;
	//top side
	indics[24] = 1;indics[25] = 5;indics[26] = 6;
	indics[27] = 1;indics[28] = 6;indics[29] = 2;
	//bottom side
	indics[30] = 4;indics[31] = 0;indics[32] = 3;
	indics[33] = 4;indics[34] = 3;indics[35] = 7;
	IB->Unlock();

	//position and aim the camera
	D3DXVECTOR3 position(0.0f, 0.0f, -6.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	_device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveLH(&proj, D3DX_PI * 0.5, (float)_nWidth / (float)_nHeight, 1.0f, 1000.0f);
	_device->SetTransform(D3DTS_PROJECTION, &proj);

	//_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);//线框模式
	_device->SetRenderState(D3DRS_LIGHTING, FALSE);//关闭灯光

	return TRUE;
}

BOOL CD3DXDeviceHelper::Display( time_t timeDeta )
{
	if (_x > D3DX_PI * 2)
	{
		_x = 0.0f;
	}
	if (_y > D3DX_PI * 2)
	{
		_y = 0.0f;
	}
	D3DXMATRIX Rx, Ry, P;
	D3DXMatrixRotationX(&Rx, _x);
	D3DXMatrixRotationY(&Ry, _y);
	D3DXMATRIX pR = Rx * Ry;

	CD3DXDeviceHelper::Instance().GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		0xffffffff, 1.0f, 0);

	_device->BeginScene();

	_device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	_device->SetIndices(IB);
	_device->SetFVF(Vertex::FVF);

	{
		_device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		D3DXMatrixTranslation(&P, -2.0f, 0.0f, 0.0f);
		D3DXMATRIX p = pR * P;
		_device->SetTransform(D3DTS_WORLD, &p);
		_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
	}

	{
		_device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		D3DXMatrixTranslation(&P, 2.0f, 0.0f, 0.0f);
		D3DXMATRIX p = pR * P;
		_device->SetTransform(D3DTS_WORLD, &p);
		_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
	}

	_device->EndScene();

	CD3DXDeviceHelper::Instance().GetDevice()->Present(NULL, NULL, NULL, NULL);
	return TRUE;
}

void CD3DXDeviceHelper::Clear()
{
	ReleaseObject<IDirect3DVertexBuffer9 *>(VB);
	ReleaseObject<IDirect3DIndexBuffer9 *>(IB);
}

void CD3DXDeviceHelper::SetRotate( float x, float y, float z )
{
	_x = x;
	_y = y;
}
