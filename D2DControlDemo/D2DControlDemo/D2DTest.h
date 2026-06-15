/*
	赛博佛珠
*/
#pragma once


#include "AtlUtil.h"
#include "StyleManager.h"
#include "D2DControl/D2DListUI.h"
#include "D2DControl/D2DExploreUI.h"
#include "D2DControl/D2DSelectUI.h"
#include "D2DControl/D2DCheckboxUI.h"

class CD2DTest : public CD2DWindow
{
public:
	static DWORD  Register();
public:
	CD2DTest();
	~CD2DTest();

	BOOL CreateUI(DWORD ExStyle, LPCWSTR lpwszWindowName,DWORD Style, int xPos, int yPos, int Cx, int Cy, HWND hwnd);
	
	bool TestCD2DScrollUI();
	void TestListView();

	VOID InitializeStyle();

	VOID InitializeStyleBox();

	VOID InitializeStyleEdit();

	VOID InitializeStyleSelect();


	VOID InitializeStyleTextArea();

	VOID InitializeStyleExplorer();


	VOID InitializeStyleDefault();


	VOID InitializeControls();

	VOID SetControlsLayout();

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

	CD2DControlUI	control;
	CD2DControlUI   control2;
	CD2DVBoxContainerUI container;
	CD2DContainerUI container1;
	CD2DContainerUI container2;

	CD2DControlUI   btn1;
	CD2DControlUI   btn2;
	CD2DControlUI   btn3;
	CD2DControlUI   btn4;
	CD2DControlUI   btn5;
	CD2DControlUI   btn6;

	
	CD2DCheckboxUI    checkbox;
	CD2DHBoxContainerUI  hbox;
	CD2DHBoxContainerUI  hbox2;
	CD2DHBoxContainerUI  hbox1;
	CD2DVBoxContainerUI  vbox;

	CD2DScrollUI         scroll;
	CD2DExploreUI		 listview;
	CD2DListUI			 list;
	CD2DTextArea         textArea;
	CD2DTextEdit         edit;
	CD2DSelectUI		 select;
	// ==================== 1. 设置全局默认样式（所有状态的 fallback） ====================
	CD2DStyle defaultStyle;
	CD2DStyle normalStyle;
	CD2DStyle hoverStyle;
	CD2DStyle checkedStyle;
	CD2DStyle disabledStyle;
	CD2DStyle pressedStyle;
	CD2DStyle focusStyle;

	CD2DStyle textAreaStyle;

	CD2DCheckboxStyle normalBoxStyle;
	CD2DCheckboxStyle hoverBoxStyle;
	CD2DCheckboxStyle checkedBoxStyle;

	CD2DSelectStyle normalSelectStyle;

	CD2DStyle containerStyle;
	CD2DStyle containerStyle1;
	CD2DStyle containerStyle2;

	CD2DItemStyle itemStyle;

	CD2DStyle editStyle;


}; 

VOID CD2DTestTest();