/*
	赛博佛珠
*/
#pragma once


#include "AtlUtil.h"
#include "D2DControl/D2D.h"
#include "StyleManager.h"



class CD2DPane : public CD2DWindow
{
public:
	static DWORD  Register();
public:
	CD2DPane();
	~CD2DPane();

	BOOL CreateUI(DWORD ExStyle, LPCWSTR lpwszWindowName,DWORD Style, int xPos, int yPos, int Cx, int Cy, HWND hwnd);
	

	VOID InitializeControls();

	VOID SetControlsLayout();

	void InitializeButton();

	void InitializeIcon();

	void InitializeTextEdit();

	void InitializeList();

	void InitializeExplorer();

	void InitializeTextArea();


public:
	BOOL CALLBACK OnCreate(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	BOOL CALLBACK OnPaint(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	BOOL CALLBACK OnSize(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	BOOL CALLBACK OnLButtonDown(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	BOOL CALLBACK OnLButtonUp(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	BOOL CALLBACK OnLButtonDown2(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	BOOL CALLBACK OnLButtonUp2(WPARAM wParam, LPARAM lParam, LRESULT& lResult);

	BOOL CALLBACK OnMouseMove(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	BOOL CALLBACK OnKeyDown(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	BOOL CALLBACK OnChar(WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	BOOL CALLBACK OnMouseWheel(WPARAM wParam, LPARAM lParam, LRESULT& lResult);

	CD2DControlUI  control;
	CD2DVBoxContainerUI root;



	CD2DExploreUI explorer;
	CD2DTextArea textArea;
	CD2DTextEdit edit;
}; 


void RegisterPaneStyles();

VOID CD2DPaneTest();