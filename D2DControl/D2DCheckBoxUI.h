#pragma once
/**
 * @brief CD2DCheckBoxUI类 - 复选框控件，支持自定义颜色和文本显示
 * 继承自CD2DControlUI和CD2DTextFormat，结合了控件基类功能和文本格式化能力
 */
class CD2DCheckBoxUI : public CD2DControlUI, public CD2DTextFormat 
{
public:
    /**
     * @brief 构造函数
     * 初始化复选框的基本属性和状态
     */
    CD2DCheckBoxUI();

    /**
     * @brief 析构函数
     * 释放相关资源
     */
    ~CD2DCheckBoxUI();

    /**
     * @brief 绘制函数
     * 绘制复选框和文本
     * @param render 渲染器指针
     */
    virtual void Render(CD2DRender* render);

    /**
     * @brief 设置和获取复选框状态
     */
    void SetChecked(bool checked);
    bool IsChecked() const;

    /**
     * @brief 设置和获取方框背景颜色
     */
    void SetBoxBackgroundColor(const D2D1_COLOR_F& color);
    D2D1_COLOR_F GetBoxBackgroundColor() const;

    /**
     * @brief 设置和获取对勾颜色
     */
    void SetCheckMarkColor(const D2D1_COLOR_F& color);
    D2D1_COLOR_F GetCheckMarkColor() const;

    /**
     * @brief 设置和获取方框边框颜色
     */
    void SetBoxBorderColor(const D2D1_COLOR_F& color);
    D2D1_COLOR_F GetBoxBorderColor() const;

    /**
     * @brief 设置和获取方框大小
     */
    void SetBoxSize(float size);
    float GetBoxSize() const;

    /**
     * @brief 设置和获取方框与文本之间的间距
     */
    void SetSpacing(float spacing);
    float GetSpacing() const;

    /**
     * @brief 鼠标事件处理 - 左键按下
     * 处理复选框点击事件，切换选中状态
     * @param point 鼠标坐标点
     * @return 是否处理了事件
     */
    virtual bool OnMouseDownL(const D2D1_POINT_2F& point) override;

    /**
     * @brief 鼠标事件处理 - 鼠标移动
     * 处理鼠标悬停效果
     * @param point 鼠标坐标点
     * @return 是否处理了事件
     */
    virtual bool OnMouseMove(const D2D1_POINT_2F& point) override;

    /**
     * @brief 鼠标事件处理 - 鼠标离开
     * 处理鼠标离开效果
     * @param point 鼠标坐标点
     * @return 是否处理了事件
     */
    virtual bool OnMouseLeave(const D2D1_POINT_2F& point) override;

    /**
     * @brief 设置选中状态改变回调函数
     * @param callback 回调函数指针
     * @param userData 用户自定义数据
     */
	 typedef bool (* OnCheckClickCallback)(CD2DCheckBoxUI* thisCheckBox, bool checked, void* userData);

    void SetOnCheckChangedCallback(OnCheckClickCallback, void* userData = nullptr);

private:
    // 复选框状态
    bool m_checked;           // 是否被选中
    bool m_mouseHover;        // 鼠标是否悬停在复选框上
    
    // 复选框外观属性
    float m_boxSize;          // 方框大小
    float m_spacing;          // 方框与文本之间的间距
    D2D1_COLOR_F m_boxBgColor; // 方框背景颜色
    D2D1_COLOR_F m_boxBorderColor; // 方框边框颜色
    D2D1_COLOR_F m_checkMarkColor; // 对勾颜色
    D2D1_COLOR_F m_boxBgColorHover; // 鼠标悬停时方框背景颜色
    D2D1_COLOR_F m_boxBorderColorHover; // 鼠标悬停时方框边框颜色
    CComPtr<IDWriteTextFormat> m_textFormat; // 文本格式化接口指针
	OnCheckClickCallback m_checkChangedCallback;// 选中状态改变回调
    
};
