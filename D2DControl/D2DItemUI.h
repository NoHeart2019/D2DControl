#pragma once
#include "D2DControlUI.h"
#include "D2DRender.h"

// 前向声明
class CD2DRender;

// Item 类
class CD2DItemUI : public CD2DControlUI
{
public:
    // 构造函数
    CD2DItemUI() 
    {
    }

    virtual ~CD2DItemUI()
    {
    }

    // 绘制方法
    virtual void Render(CD2DRender* pRender)
    {
        // 绘制背景
        if (IsSelected())
        {
            pRender->FillRectangle(m_rectangle, D2D1::ColorF(0.0f, 0.5f, 1.0f));
        }
        else if (IsHovered())
        {
            pRender->FillRectangle(m_rectangle, D2D1::ColorF(0.7f, 0.7f, 0.7f));
        }
        else
        {
            pRender->FillRectangle(m_rectangle, D2D1::ColorF(0.9f, 0.9f, 0.9f));
        }

        // 绘制边框
        pRender->DrawRectangle(m_rectangle, D2D1::ColorF(0.5f, 0.5f, 0.5f), 1.0f);
    }

};
