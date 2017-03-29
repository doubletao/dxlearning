#pragma once
class CD3DXDeviceHelper
{
private:
	CD3DXDeviceHelper(void);
	~CD3DXDeviceHelper(void);
private:
	//windows成员
	HINSTANCE _hInst;
	HWND _hWnd;
	int _nWidth;
	int _nHeight;
	BOOL _bWindowed;

	float _xObj; //物件的x旋转
	float _yObj; //物件的y旋转

	float _xCamera; //摄像机的x旋转
	float _yCamera; //摄像机的y旋转
	float _zCamera; //摄像机的y旋转

	float _distanceCamera;

	//D3D成员
	IDirect3D9 * _d3d9;//接口指针
	D3DCAPS9 _cap9;//显卡支持属性集
	int _vp;//顶点运算类型
	D3DPRESENT_PARAMETERS _d3dParam;//设备接口创建参数
	IDirect3DDevice9 * _device;//设备指针
	IDirect3DVertexBuffer9 * VB;
	IDirect3DIndexBuffer9 * IB;
	IDirect3DTexture9 * _Tex;
	IDirect3DTexture9 * _TexWall;
	IDirect3DTexture9 * _TexFloor;

	BOOL bInitialized;
public:
	static D3DXVECTOR3 CalNormalVec3(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3);
public:
	void SetBasicInfomation(HINSTANCE hInst, HWND hWnd, int nWidth, int nHeight, BOOL bWindowed);
	BOOL InitialD3D();
public:
	static CD3DXDeviceHelper & Instance();
	IDirect3D9 * GetD3D();
	IDirect3DDevice9 * GetDevice();
	BOOL Initialized();
	void SetRotate(float x, float y, float z);
	void RotateCamera(float x, float y, float z);
	void SetCameraDistance(float fDis);
	void GetRotate(float &x, float &y, float &z);
	void GetRotateCamera(float &x, float &y, float &z);
	void GetCameraDistance(float &fDis);
	BOOL SetupColorSample();
	BOOL SetupLightSample();
	BOOL SetupTextureSample();
	BOOL SetupMirrorSample();
	BOOL DisplayColorSample(time_t timeDeta);
	BOOL DisplayLightSample(time_t timeDeta);
	BOOL DisplayTextureSample(time_t timeDeta);
	BOOL DisplayMirrorSample(time_t timeDeta);
	void Clear();
};

template<class T> void ReleaseObject(T t)
{
	if (t)
	{
		t->Release();
		t = NULL;
	}
}
template<class T> void DeleteObject(T t)
{
	if (t)
	{
		delete t;
		t = NULL;
	}
}


