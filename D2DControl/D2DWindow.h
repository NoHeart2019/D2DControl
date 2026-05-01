#pragma once

class CD2DWindow : public CD2DContainerUI, public CD2DRender, public C32Window
{
public:
	static DWORD  Register();

public:
	CD2DWindow();
	~CD2DWindow();

public:
	virtual VOID Render();
	BOOL CreateUI(DWORD ExStyle, LPCWSTR lpwszWindowName,DWORD Style, int xPos, int yPos, int Cx, int Cy, HWND hwnd);

	virtual VOID InitializeControls();

	virtual VOID SetControlsRectangles();

public:
	virtual BOOL CALLBACK OnCreate(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	virtual BOOL CALLBACK OnPaint(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	virtual BOOL CALLBACK OnSize(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	virtual BOOL CALLBACK OnLButtonDown(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	virtual BOOL CALLBACK OnLButtonUp(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	virtual BOOL CALLBACK OnMouseMove(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	virtual BOOL CALLBACK OnLButtonDown2(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	virtual BOOL CALLBACK OnDisplayChange(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	virtual BOOL CALLBACK OnCaptureChanged(WPARAM wParam, LPARAM lParam, LRESULT& lResult);


	//根据Client域大小设置整个window 大小
	VOID SetWindowSize(float width, float height);

	
}; 
