#include "StdAfx.h"
#include "D3DXDeviceHelper.h"

struct VertexColor
{
	float _x;
	float _y;
	float _z;
	D3DCOLOR _color;
	VertexColor(){}
	VertexColor(float x, float y, float z, D3DCOLOR color)
		: _x(x)
		, _y(y)
		, _z(z)
		, _color(color)
	{
	}
	static const DWORD FVF;
};
const DWORD VertexColor::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

struct VertexNormal
{
	float _x;
	float _y;
	float _z;
	float _nx;
	float _ny;
	float _nz;
	VertexNormal(){}
	VertexNormal(float x, float y, float z, float nx, float ny, float nz)
		: _x(x), _y(y), _z(z), _nx(nx), _ny(ny), _nz(nz)
	{}
	static const WORD FVF;
};
const WORD VertexNormal::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

struct VertexTexture
{
	float _x;
	float _y;
	float _z;
	float _nx;
	float _ny;
	float _nz;
	float _u;
	float _v;
	VertexTexture(){}
	VertexTexture(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		: _x(x), _y(y), _z(z), _nx(nx), _ny(ny), _nz(nz), _u(u), _v(v)
	{}
	static const WORD FVF;
};
const WORD VertexTexture::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

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
	, _Tex(NULL)
	, _TexWall(NULL)
	, _TexFloor(NULL)
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

BOOL CD3DXDeviceHelper::SetupColorSample()
{
	_device->CreateVertexBuffer(8 * sizeof(VertexColor), D3DUSAGE_WRITEONLY, VertexColor::FVF, D3DPOOL_MANAGED, &VB, 0);
	_device->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &IB, 0);

	VertexColor * vertices = NULL;
	VB->Lock(0, 0, (void **)&vertices, 0);
	vertices[0] = VertexColor(-1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(255, 0, 0));
	vertices[1] = VertexColor(-1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(255, 255, 0));
	vertices[2] = VertexColor(1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(255, 255, 255));
	vertices[3] = VertexColor(1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0, 255, 255));
	vertices[4] = VertexColor(-1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 255));
	vertices[5] = VertexColor(-1.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0));
	vertices[6] = VertexColor(1.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 0));
	vertices[7] = VertexColor(1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 0));
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

BOOL CD3DXDeviceHelper::DisplayColorSample( time_t timeDeta )
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

	_device->SetStreamSource(0, VB, 0, sizeof(VertexColor));
	_device->SetIndices(IB);
	_device->SetFVF(VertexColor::FVF);

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

BOOL CD3DXDeviceHelper::SetupLightSample()
{
	//启用光照
	_device->SetRenderState(D3DRS_LIGHTING, true);

	//创建顶点缓存
	_device->CreateVertexBuffer(18 * sizeof(VertexNormal), D3DUSAGE_WRITEONLY, VertexNormal::FVF, D3DPOOL_MANAGED, &VB, NULL);

#if 0
	//求出底边顶点的法向量
	D3DXVECTOR3 v1(-1.0f, 0.0f, -1.0f);
	D3DXVECTOR3 v2(1.0f, 0.0f, -1.0f);
	D3DXVECTOR3 v3(1.0f, 0.0f, 1.0f);
	D3DXVECTOR3 vF = v2 - v1;
	D3DXVECTOR3 vS = v3 - v1;
	D3DXVECTOR3 vN(0.0f, 0.0f, 0.0f);
	D3DXVec3Cross(&vN, &vF, &vS);
#endif

	//金字塔
	VertexNormal * v;
	VB->Lock(0, 0, (void **)&v, 0);
	//front
	v[0] = VertexNormal(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
	v[1] = VertexNormal(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, -0.707f);
	v[2] = VertexNormal(1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
	//left
	v[3] = VertexNormal(-1.0f, 0.0f, 1.0f, -0.707f, 0.707f, 0.0f);
	v[4] = VertexNormal(0.0f, 1.0f, 0.0f, -0.707f, 0.707f, 0.0f);
	v[5] = VertexNormal(-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f);
	//right
	v[6] = VertexNormal(1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0f);
	v[7] = VertexNormal(0.0f, 1.0f, 0.0f, 0.707f, 0.707f, 0.0f);
	v[8] = VertexNormal(1.0f, 0.0f, 1.0f, 0.707f, 0.707f, 0.0f);
	//back
	v[9] = VertexNormal(1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);
	v[10] = VertexNormal(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, 0.707f);
	v[11] = VertexNormal(-1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);
	//bottom
	v[12] = VertexNormal(-1.0f, 0.0f, -1.0f, 0.0f, -4.0f, 0.0f);
	v[13] = VertexNormal(1.0f, 0.0f, -1.0f, 0.0f, -4.0f, 0.0f);
	v[14] = VertexNormal(1.0f, 0.0f, 1.0f, 0.0f, -4.0f, 0.0f);
	v[15] = VertexNormal(1.0f, 0.0f, 1.0f, 0.0f, -4.0f, 0.0f);
	v[16] = VertexNormal(-1.0f, 0.0f, 1.0f, 0.0f, -4.0f, 0.0f);
	v[17] = VertexNormal(-1.0f, 0.0f, -1.0f, 0.0f, -4.0f, 0.0f);
	VB->Unlock();

	//设置材质
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);//ambient环境光
	mtrl.Diffuse = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);//diffuse漫反射
	mtrl.Specular = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);//specular高光
	mtrl.Emissive = (D3DXCOLOR)D3DCOLOR_XRGB(0, 0, 0);//emissive自发光
	mtrl.Power = 5.0f;
	_device->SetMaterial(&mtrl);

	//方向光(左)
	D3DLIGHT9 dirLight;
	::ZeroMemory(&dirLight, sizeof(D3DLIGHT9));
	dirLight.Type = D3DLIGHT_DIRECTIONAL;
	dirLight.Diffuse = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);
	dirLight.Specular = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255) * 0.1f;
	dirLight.Ambient = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255) * 0.0f;
	dirLight.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	_device->SetLight(0, &dirLight);
	_device->LightEnable(0, true);

	//点光源(右)
	D3DLIGHT9 ptLight;
	::ZeroMemory(&ptLight, sizeof(D3DLIGHT9));
	ptLight.Type = D3DLIGHT_POINT;
	ptLight.Diffuse = (D3DXCOLOR)D3DCOLOR_XRGB(255, 0, 0);
	ptLight.Specular = (D3DXCOLOR)D3DCOLOR_XRGB(255, 0, 0) * 0.1f;
	ptLight.Ambient = (D3DXCOLOR)D3DCOLOR_XRGB(255, 0, 0) * 0.0f;
	ptLight.Position = D3DXVECTOR3(4.0f, 0.0f, 0.0f);
	ptLight.Range = 8.0f;
	_device->SetLight(1, &ptLight);
	_device->LightEnable(1, true);

	//聚光灯(顶)
	D3DLIGHT9 spotLight;
	::ZeroMemory(&spotLight, sizeof(D3DLIGHT9));
	spotLight.Type = D3DLIGHT_SPOT;
	spotLight.Diffuse = (D3DXCOLOR)D3DCOLOR_XRGB(0, 0, 255);
	spotLight.Specular = (D3DXCOLOR)D3DCOLOR_XRGB(0, 0, 255) * 0.1f;
	spotLight.Ambient = (D3DXCOLOR)D3DCOLOR_XRGB(0, 0, 255) * 0.0f;
	spotLight.Position = D3DXVECTOR3(0.0f, 2.0f, 0.0f);
	spotLight.Range = 8.0f;
	spotLight.Falloff = 2.0f;
	spotLight.Direction = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	spotLight.Theta = D3DX_PI / 3;
	spotLight.Phi = D3DX_PI / 2;
	_device->SetLight(2, &spotLight);
	_device->LightEnable(2, true);

	_device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	_device->SetRenderState(D3DRS_SPECULARENABLE, true);

	D3DXVECTOR3 position(0.0f, 0.0f, -6.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	_device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveLH(&proj, D3DX_PI * 0.5, (float)_nWidth / (float)_nHeight, 1.0f, 1000.0f);
	_device->SetTransform(D3DTS_PROJECTION, &proj);

	return TRUE;
}

BOOL CD3DXDeviceHelper::DisplayLightSample( time_t timeDeta )
{
	if (_x > D3DX_PI * 2)
	{
		_x = 0.0f;
	}
	if (_y > D3DX_PI * 2)
	{
		_y = 0.0f;
	}
	D3DXMATRIX Rx, Ry;
	D3DXMatrixRotationX(&Rx, _x);
	D3DXMatrixRotationY(&Ry, _y);
	D3DXMATRIX pR = Rx * Ry;

	_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	_device->BeginScene();

	_device->SetStreamSource(0, VB, 0, sizeof(VertexNormal));
	_device->SetFVF(VertexNormal::FVF);

	{
		D3DXMATRIX Pos;
		D3DXMatrixTranslation(&Pos, 2.0f, 0.0f, 0.0f);
		pR *= Pos;
		_device->SetTransform(D3DTS_WORLD, &pR);
		_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 18);
	}

	{
		D3DXMATRIX Pos;
		D3DXMatrixTranslation(&Pos, -3.0f, 0.0f, 0.0f);
		pR *= Pos;
		_device->SetTransform(D3DTS_WORLD, &pR);
		ID3DXMesh * teaPot = NULL;
		D3DXCreateTeapot(_device, &teaPot, NULL);
		teaPot->DrawSubset(0);
		ReleaseObject<ID3DXMesh *>(teaPot);
	}

	_device->EndScene();
	_device->Present(NULL, NULL, NULL, NULL);
	return TRUE;
}

BOOL CD3DXDeviceHelper::SetupTextureSample()
{
	_device->CreateVertexBuffer(12 * sizeof(VertexTexture), D3DUSAGE_WRITEONLY, VertexTexture::FVF, D3DPOOL_MANAGED, &VB, NULL);
	VertexTexture * v;
	VB->Lock(0, 0, (void **)&v, 0);

	v[0] = VertexTexture(-2.36f, -4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = VertexTexture(-2.36f, 4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = VertexTexture(2.36f, 4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[3] = VertexTexture(-2.36f, -4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = VertexTexture(2.36f, 4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = VertexTexture(2.36f, -4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	v[8] = VertexTexture(-2.36f, -4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7] = VertexTexture(-2.36f, 4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[6] = VertexTexture(2.36f, 4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = VertexTexture(-2.36f, -4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = VertexTexture(2.36f, 4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[9] = VertexTexture(2.36f, -4.208f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	VB->Unlock();

	D3DXCreateTextureFromFile(_device, _T("D:\\DXLearning\\pic\\1.jpg"), &_Tex);

	_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	
	_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	{
		//点光源(前)
		D3DLIGHT9 ptLight;
		::ZeroMemory(&ptLight, sizeof(D3DLIGHT9));
		ptLight.Type = D3DLIGHT_POINT;
		ptLight.Diffuse = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);
		ptLight.Specular = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255) * 0.1f;
		ptLight.Ambient = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255) * 0.0f;
		ptLight.Position = D3DXVECTOR3(0.0f, 0.0f, -2.0f);
		ptLight.Range = 8.0f;
		_device->SetLight(0, &ptLight);
		_device->LightEnable(0, true);
	}

	{
		//点光源(后)
		D3DLIGHT9 ptLight;
		::ZeroMemory(&ptLight, sizeof(D3DLIGHT9));
		ptLight.Type = D3DLIGHT_POINT;
		ptLight.Diffuse = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);
		ptLight.Specular = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255) * 0.1f;
		ptLight.Ambient = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255) * 0.0f;
		ptLight.Position = D3DXVECTOR3(0.0f, 0.0f, 2.0f);
		ptLight.Range = 8.0f;
		_device->SetLight(1, &ptLight);
		_device->LightEnable(1, true);
	}

	D3DXVECTOR3 position(0.0f, 0.0f, -6.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	_device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveLH(&proj, D3DX_PI * 0.5, (float)_nWidth / (float)_nHeight, 1.0f, 1000.0f);
	_device->SetTransform(D3DTS_PROJECTION, &proj);

	return TRUE;
}

BOOL CD3DXDeviceHelper::DisplayTextureSample( time_t timeDeta )
{
	if (_device)
	{
		if (_x > D3DX_PI * 2)
		{
			_x = 0.0f;
		}
		if (_y > D3DX_PI * 2)
		{
			_y = 0.0f;
		}
		D3DXMATRIX Rx, Ry;
		D3DXMatrixRotationX(&Rx, _x);
		D3DXMatrixRotationY(&Ry, _y);
		D3DXMATRIX pR = Rx * Ry;

		_device->Clear(0, 0, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET | D3DCLEAR_STENCIL, 0xffffffff, 1.0f, 0);
		_device->BeginScene();

		_device->SetStreamSource(0, VB, 0, sizeof(VertexTexture));
		_device->SetFVF(VertexTexture::FVF);

		{
			_device->SetRenderState(D3DRS_LIGHTING, true);
			D3DXMATRIX Pos;
			D3DXMatrixTranslation(&Pos, 0.0f, 0.0f, 0.0f);
			pR *= Pos;
			_device->SetTransform(D3DTS_WORLD, &pR);
			//设置材质
			D3DMATERIAL9 mtrl;
			mtrl.Ambient = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);//ambient环境光
			mtrl.Diffuse = (D3DXCOLOR)D3DCOLOR_ARGB(255, 255, 255, 255);//diffuse漫反射
			mtrl.Specular = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);//specular高光
			mtrl.Emissive = (D3DXCOLOR)D3DCOLOR_XRGB(0, 0, 0);//emissive自发光
			mtrl.Power = 5.0f;
			_device->SetMaterial(&mtrl);
			_device->SetTexture(0, _Tex);
			_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);
		}

		{
			_device->SetRenderState(D3DRS_LIGHTING, true);
			_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			D3DXMATRIX Pos;
			D3DXMatrixTranslation(&Pos, 0.0f, 0.0f, 0.0f);
			pR *= Pos;
			_device->SetTransform(D3DTS_WORLD, &pR);
			//设置材质
			D3DMATERIAL9 mtrl;
			mtrl.Ambient = (D3DXCOLOR)D3DCOLOR_XRGB(255, 0, 0);//ambient环境光
			mtrl.Diffuse = (D3DXCOLOR)D3DCOLOR_ARGB(200, 255, 0, 0);//diffuse漫反射
			mtrl.Specular = (D3DXCOLOR)D3DCOLOR_XRGB(255, 0, 0);//specular高光
			mtrl.Emissive = (D3DXCOLOR)D3DCOLOR_XRGB(255, 0, 0);//emissive自发光
			mtrl.Power = 5.0f;
			_device->SetMaterial(&mtrl);
			_device->SetTexture(0, 0);
			
			ID3DXMesh * pMesh = NULL;
			D3DXCreateCylinder(_device, 1.0f, 1.3f, 1.5f, 8, 4, &pMesh, NULL);
			pMesh->DrawSubset(0);

			_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		}

		_device->EndScene();

		_device->Present(NULL, NULL, 0, NULL);
	}
	return TRUE;
}

D3DXVECTOR3 CD3DXDeviceHelper::CalNormalVec3( D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3 )
{
	D3DXVECTOR3 vN(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vF = v2 - v1;
	D3DXVECTOR3 vS = v3 - v1;
	D3DXVec3Cross(&vN, &vF, &vS);
	return vN;
}

BOOL CD3DXDeviceHelper::SetupMirrorSample()
{
	//加载纹理
	D3DXCreateTextureFromFile(_device, _T("D:\\DXLearning\\pic\\wall.jpg"), &_TexWall);
	D3DXCreateTextureFromFile(_device, _T("D:\\DXLearning\\pic\\floor.jpg"), &_TexFloor);

	//开灯
	_device->SetRenderState(D3DRS_LIGHTING, true);

	//设置灯光
	{
		//点光源(前)
		D3DLIGHT9 ptLight;
		::ZeroMemory(&ptLight, sizeof(D3DLIGHT9));
		ptLight.Type = D3DLIGHT_POINT;
		ptLight.Diffuse = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);
		ptLight.Specular = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255) * 0.1f;
		ptLight.Ambient = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255) * 0.0f;
		ptLight.Position = D3DXVECTOR3(0.0f, 0.0f, -6.0f);
		ptLight.Range = 9.0f;
		_device->SetLight(0, &ptLight);
		_device->LightEnable(0, true);
	}

	//设定摄像机矩阵
	D3DXMATRIX camera;
	D3DXVECTOR3 position(-4.0f, 2.0f, -7.0f);
	D3DXVECTOR3 lookat(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&camera, &position, &lookat, &up);
	_device->SetTransform(D3DTS_VIEW, &camera);

	_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);

	//设定投影矩阵
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveLH(&proj, D3DX_PI * 0.5, float(_nWidth) / float(_nHeight), 1.0f, 1000.0f);
	_device->SetTransform(D3DTS_PROJECTION, &proj);

	return TRUE;
}

BOOL CD3DXDeviceHelper::DisplayMirrorSample( time_t timeDeta )
{
	_x = 0.0f;
	//旋转逻辑
	if (_x > D3DX_PI * 2)
	{
		_x = 0.0f;
	}
	if (_y > D3DX_PI * 2)
	{
		_y = 0.0f;
	}
	D3DXMATRIX Rx, Ry;
	D3DXMatrixRotationX(&Rx, _x);
	D3DXMatrixRotationY(&Ry, _y);
	D3DXMATRIX pR = Rx * Ry;

	_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, 0xff00ff00, 1.0f, 0);
	_device->BeginScene();

	//画墙和地板
	{
		//墙的材质
		D3DMATERIAL9 mtrWall;
		mtrWall.Ambient = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);//ambient环境光
		mtrWall.Diffuse = (D3DXCOLOR)D3DCOLOR_ARGB(255, 255, 255, 255);//diffuse漫反射
		mtrWall.Specular = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);//specular高光
		mtrWall.Emissive = (D3DXCOLOR)D3DCOLOR_XRGB(0, 0, 0);//emissive自发光
		mtrWall.Power = 5.0f;
		_device->SetMaterial(&mtrWall);
		//墙的纹理
		_device->SetTexture(0, _TexWall);
		//墙的矩阵
		D3DXMATRIX pos;
		D3DXMatrixTranslation(&pos, 0.0f, 2.0f, 0.0f);
		pos = pR * pos;
		_device->SetTransform(D3DTS_WORLD, &pos);
		//画墙
		ID3DXMesh * pMeshWall = NULL;
		D3DXCreateBox(_device, 8.0f, 5.0f, 0.1f, &pMeshWall, NULL);
		pMeshWall->DrawSubset(0);
	}

	{
		//地板的材质
		D3DMATERIAL9 mtrFloor;
		mtrFloor.Ambient = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);//ambient环境光
		mtrFloor.Diffuse = (D3DXCOLOR)D3DCOLOR_ARGB(255, 255, 255, 255);//diffuse漫反射
		mtrFloor.Specular = (D3DXCOLOR)D3DCOLOR_XRGB(255, 255, 255);//specular高光
		mtrFloor.Emissive = (D3DXCOLOR)D3DCOLOR_XRGB(0, 0, 0);//emissive自发光
		mtrFloor.Power = 5.0f;
		_device->SetMaterial(&mtrFloor);
		//地板的纹理
		_device->SetTexture(0, _TexFloor);
		//地板的矩阵
		float x1 = _x + D3DX_PI * 0.5;
		if (x1 > D3DX_PI * 2)
		{
			x1 = 0.0f;
		}
		D3DXMATRIX Rx, Ry;
		D3DXMatrixRotationX(&Rx, x1);
		D3DXMatrixRotationY(&Ry, _y);
		D3DXMATRIX pR = Rx * Ry;
		D3DXMATRIX pos;
		D3DXMatrixTranslation(&pos, 0.0f, -0.5f, 0.0f);
		pos = pR * pos;
		_device->SetTransform(D3DTS_WORLD, &pos);
		//画地板
		ID3DXMesh * pMeshWall = NULL;
		D3DXCreateBox(_device, 8.0f, 5.0f, 0.1f, &pMeshWall, NULL);
		pMeshWall->DrawSubset(0);
	}

	{
		//画个镜子
	}
	_device->EndScene();
	_device->Present(NULL, NULL, 0, NULL);

	return TRUE;
}
