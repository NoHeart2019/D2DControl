#include "stdafx.h"
#include "D2D.h"
#include "D2DWindow.h"


DWORD CD2DWindow::Register()
{
	return RegisterClassEx32(L"CD2DWindow");
}

CD2DWindow::CD2DWindow()
{
	
}

CD2DWindow::~CD2DWindow()
{

}

VOID CD2DWindow::InitializeControls()
{
	
}

VOID CD2DWindow::SetControlsRectangles()
{
	D2D1_RECT_F rectangle = GetRectangle();
	D2D1_SIZE_F size = GetSize();
}

BOOL CD2DWindow::CreateUI(DWORD ExStyle, LPCWSTR lpwszWindowName,DWORD Style, int xPos, int yPos, int Cx, int Cy, HWND hwnd)
{

	Create(ExStyle, L"CD2DWindow", lpwszWindowName, Style, xPos, yPos, Cx, Cy, hwnd);

	return (m_Hwnd != NULL);
}


VOID CD2DWindow::SetWindowSize(float width, float height)
{
	SIZE size = GetNoClientSize(m_Hwnd);

	SetPos(0, 0, (int)ScaleX(width, false)+size.cx, (int)ScaleY(height, false)+size.cy, SWP_NOMOVE|SWP_NOZORDER);
}

VOID CD2DWindow::Render()
{
	CComPtr<ID2D1LinearGradientBrush> brush = CreateLinearBrush(GetRectangle(), m_LinearColor[0], m_LinearColor[1]);
	GetRenderTarget()->FillRoundedRectangle(GetRoundedRectangle(), brush);
}

BOOL CALLBACK CD2DWindow::OnCreate(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	Initialize(m_Hwnd);	
	InitializeControls();
	SetControlsRectangles();
	return TRUE;
}

BOOL CALLBACK CD2DWindow::OnPaint(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	PAINTSTRUCT ps;
	BeginPaint(m_Hwnd, &ps);
	BeginDraw();
	Clear(D2D1::ColorF(D2D1::ColorF::White));
	Render();

	EndDraw();

	EndPaint(m_Hwnd, &ps);

	return TRUE;
}

BOOL CALLBACK CD2DWindow::OnSize(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	float width = GET_X_LPARAM_F(lParam);
	float height = GET_Y_LPARAM_F(lParam);
	CD2DRender::Resize(width, height);
	SetPosition(0.0f, 0.0f);
	SetSize(ScaleX(width, TRUE), ScaleY(height, TRUE));
	SetControlsRectangles();
	return TRUE;
}

BOOL CALLBACK CD2DWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F(ScaleX((float)GET_X_LPARAM_F(lParam), TRUE),
		ScaleY((float)GET_Y_LPARAM_F(lParam), TRUE));
	if(CD2DContainerUI::OnMouseDownL(point) == true)
		InvalidateRect(NULL, FALSE);
	SetCapture(m_Hwnd);
	return TRUE;
}

BOOL CALLBACK CD2DWindow::OnLButtonUp(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F((float)ScaleX(GET_X_LPARAM_F(lParam), TRUE),
		(float)ScaleY(GET_Y_LPARAM_F(lParam), TRUE));
	if(CD2DContainerUI::OnMouseUpL(point) == true)
		InvalidateRect(NULL, FALSE);
	ReleaseCapture();
	return TRUE;
}

BOOL CALLBACK CD2DWindow::OnMouseMove(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F(ScaleX((float)GET_X_LPARAM_F(lParam), TRUE),
		ScaleY((float)GET_Y_LPARAM_F(lParam), TRUE));
	if(CD2DContainerUI::OnMouseMove(point) == true)
		InvalidateRect(NULL, FALSE);

	return TRUE;
}

BOOL CALLBACK CD2DWindow::OnLButtonDown2(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	D2D1_POINT_2F point = D2D1::Point2F(ScaleX((float)GET_X_LPARAM_F(lParam), TRUE),
		ScaleY((float)GET_Y_LPARAM_F(lParam), TRUE));
	if(CD2DContainerUI::OnMouseDownL2(point) == true)
		InvalidateRect(NULL, FALSE);
	return TRUE;
}

BOOL CALLBACK CD2DWindow::OnDisplayChange(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	int dpiX = GET_X_LPARAM(lParam);
	int dpiY = GET_Y_LPARAM(lParam);

	float scaleX = (dpiX/96.0f);
	float scaleY = (dpiY/96.0f);

	SetDpiScale(scaleX, scaleY);
	SIZE Size = GetClientSize();
	SetWindowSize((float)Size.cx, (float)Size.cy);
	return TRUE;
}

BOOL CALLBACK CD2DWindow::OnCaptureChanged(WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	HWND hwnd = (HWND)lParam;
	return TRUE;
}

