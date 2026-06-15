#include "stdafx.h"

#include "StyleManager.h"

BOOL StyleManager::bRegistered = FALSE;


void StyleManager::Reigster()
{
	auto& styleMgr = StyleManager::Get();

	// ========== 注册样式类（只执行一次，可放在 InitInstance 或静态构造函数）==========
	if (!bRegistered)
	{
		// ----- 蓝色按钮样式 -----
		StyleManager::ButtonStyleSet blueBtn;
		blueBtn.normal.background.m_type = CD2DBackground::Solid;
		blueBtn.normal.background.m_solid.color = D2D1::ColorF(0.8f, 0.8f, 1.0f);
		blueBtn.normal.border.m_top = CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.4f, 0.4f, 0.4f));
		blueBtn.normal.border.m_right = blueBtn.normal.border.m_top;
		blueBtn.normal.border.m_bottom = blueBtn.normal.border.m_top;
		blueBtn.normal.border.m_left = blueBtn.normal.border.m_top;
		blueBtn.normal.radius = CD2DRadius(0.5f);
		blueBtn.normal.margin = CD2DMargin(12.0f);
		blueBtn.normal.padding = CD2DPadding(8.0f);

		blueBtn.normal.text.color = D2D1::ColorF(0.0f, 0.0f, 0.0f);
		blueBtn.normal.text.fontFamily = L"Segoe UI";
		blueBtn.normal.text.fontSize = 14.0f;
		blueBtn.normal.text.horizontalAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
		blueBtn.normal.text.verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
	


		// Hover
		blueBtn.hover.background.m_type = CD2DBackground::Solid;
		blueBtn.hover.background.m_solid.color = D2D1::ColorF(0.88f, 0.88f, 1.1f);
		blueBtn.hover.border.m_top = CD2DBorderEdge(1.5f, Solid, D2D1::ColorF(0.2f, 0.6f, 1.0f));
		blueBtn.hover.border.m_right = blueBtn.hover.border.m_top;
		blueBtn.hover.border.m_bottom = blueBtn.hover.border.m_top;
		blueBtn.hover.border.m_left = blueBtn.hover.border.m_top;
		blueBtn.hover.transform.SetTranslate(0.0f, -4.0f);


		// Pressed
		blueBtn.pressed.background.m_type = CD2DBackground::Solid;
		blueBtn.pressed.background.m_solid.color = D2D1::ColorF(0.64f, 0.64f, 0.8f);
		styleMgr.RegisterButtonStyle(L"blue-button", blueBtn);

		// ----- 列表项默认样式 -----
		StyleManager::ListItemStyleSet listItem;
		listItem.normal.background.m_type = CD2DBackground::Solid;
		listItem.normal.background.m_solid.color = D2D1::ColorF(1.0f, 1.0f, 1.0f);
		listItem.normal.border.m_top = CD2DBorderEdge(1.0f, Solid, D2D1::ColorF(0.8f, 0.8f, 0.8f));
		listItem.normal.border.m_right = listItem.normal.border.m_top;
		listItem.normal.border.m_bottom = listItem.normal.border.m_top;
		listItem.normal.border.m_left = listItem.normal.border.m_top;
		listItem.normal.radius = CD2DRadius(4.0f);
		listItem.normal.text.color = D2D1::ColorF(0.1f, 0.1f, 0.1f);
		listItem.normal.text.fontFamily = L"Segoe UI";
		listItem.normal.text.fontSize = 14.0f;
		// Hover
		listItem.hover.background.m_type = CD2DBackground::Solid;
		listItem.hover.background.m_solid.color = D2D1::ColorF(0.85f, 0.92f, 1.0f);
		listItem.hover.border.m_top = CD2DBorderEdge(1.0f, Solid, D2D1::ColorF(0.2f, 0.5f, 1.0f));
		listItem.hover.border.m_right = listItem.hover.border.m_top;
		listItem.hover.border.m_bottom = listItem.hover.border.m_top;
		listItem.hover.border.m_left = listItem.hover.border.m_top;
		listItem.hover.transform.SetTranslate(0.0f, -4.0f);
		// Checked
		listItem.checked.background.m_type = CD2DBackground::Solid;
		listItem.checked.background.m_solid.color = D2D1::ColorF(0.85f, 1.0f, 0.85f);
		listItem.checked.border.m_top = CD2DBorderEdge(1.0f, Solid, D2D1::ColorF(0.2f, 0.8f, 0.2f));
		// DragOver
		listItem.dragOver.background.m_type = CD2DBackground::Solid;
		listItem.dragOver.background.m_solid.color = D2D1::ColorF(0.8f, 0.8f, 1.0f, 0.8f);
		listItem.dragOver.border.m_top = CD2DBorderEdge(2.0f, Solid, D2D1::ColorF(0.2f, 0.5f, 1.0f));
		listItem.dragOver.transform.SetScale(1.1f, 1.1f); 
		
		// Dragging
		listItem.Dragging.background.m_type = CD2DBackground::Solid;
		listItem.Dragging.background.m_solid.color = D2D1::ColorF(0.9f, 0.9f, 1.0f, 0.8f);
		listItem.Dragging.border.m_top = CD2DBorderEdge(2.0f, Solid, D2D1::ColorF(0.2f, 0.5f, 1.0f));
		

		styleMgr.RegisterListItemStyle(L"default-item", listItem);

		bRegistered = TRUE;
		}



}
