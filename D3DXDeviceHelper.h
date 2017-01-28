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

	float _x; 
	float _y;

	//D3D成员
	IDirect3D9 * _d3d9;//接口指针
	D3DCAPS9 _cap9;//显卡支持属性集
	int _vp;//顶点运算类型
	D3DPRESENT_PARAMETERS _d3dParam;//设备接口创建参数
	IDirect3DDevice9 * _device;//设备指针
	IDirect3DVertexBuffer9 * VB;
	IDirect3DIndexBuffer9 * IB;

	BOOL bInitialized;
public:
	void SetBasicInfomation(HINSTANCE hInst, HWND hWnd, int nWidth, int nHeight, BOOL bWindowed);
	BOOL InitialD3D();
public:
	static CD3DXDeviceHelper & Instance();
	IDirect3D9 * GetD3D();
	IDirect3DDevice9 * GetDevice();
	BOOL Initialized();
	void SetRotate(float x, float y, float z);
	BOOL Setup();
	BOOL Display(time_t timeDeta);
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


