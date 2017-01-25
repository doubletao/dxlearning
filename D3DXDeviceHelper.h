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

	//D3D成员
	IDirect3D9 * _d3d9;//接口指针
	D3DCAPS9 _cap9;//显卡支持属性集
	int _vp;//顶点运算类型
	D3DPRESENT_PARAMETERS _d3dParam;//设备接口创建参数
	IDirect3DDevice9 * _device;//设备指针
public:
	void SetBasicInfomation(HINSTANCE hInst, HWND hWnd, int nWidth, int nHeight, BOOL bWindowed);
	BOOL InitialD3D();
public:
	static CD3DXDeviceHelper & Instance();
};

