#pragma once
class CD2DAlignment
{
public:
	// 完整对齐标志（水平+垂直）
	enum D2D_ALIGNMENT
	{
		ALIGN_NONE   = 0x00,

		// 水平
		ALIGN_LEFT   = 0x01,
		ALIGN_CENTER = 0x02,   // 水平居中
		ALIGN_RIGHT  = 0x03,

		// 垂直
		ALIGN_TOP    = 0x04,
		ALIGN_MIDDLE = 0x08,   // 垂直居中（注意：VS2010 中 CENTER 可能与 windows.h 宏冲突，改用 MIDDLE）
		ALIGN_BOTTOM = 0x0C
	};

public:
	CD2DAlignment() : m_horz(ALIGN_LEFT), m_vert(ALIGN_TOP) {}

	CD2DAlignment(D2D_ALIGNMENT horz, D2D_ALIGNMENT vert)
		: m_horz(horz), m_vert(vert)
	{
		// 简单防错：如果传入了无效组合，可以矫正
	}

	void SetHorizontal(D2D_ALIGNMENT horz) { m_horz = horz; }
	void SetVertical(D2D_ALIGNMENT vert)   { m_vert = vert; }
	D2D_ALIGNMENT GetHorizontal()          { return m_horz; }
	D2D_ALIGNMENT GetVertical()			   { return m_vert; }

	// 便捷方法：同时设置为居中
	void SetCenter()
	{
		m_horz = ALIGN_CENTER;
		m_vert = ALIGN_MIDDLE;
	}

	// 核心计算：返回内容左上角坐标
	D2D1_POINT_2F CalculatePosition(const D2D1_RECT_F& container, const D2D1_SIZE_F& contentSize) const
	{
		float x = container.left;
		float y = container.top;

		// 水平
		if (m_horz == ALIGN_LEFT)
			x = container.left;
		else if (m_horz == ALIGN_RIGHT)
			x = container.right - contentSize.width;
		else if (m_horz == ALIGN_CENTER)
			x = (container.right + container.left)/2.0f  - contentSize.width / 2.0f;

		// 垂直
		if (m_vert == ALIGN_TOP)
			y = container.top;
		else if (m_vert == ALIGN_BOTTOM)
			y = container.bottom - contentSize.height;
		else if (m_vert == ALIGN_MIDDLE)
			y = (container.bottom +container.top)/2.0f  - contentSize.height / 2.0f;

		return D2D1::Point2F(x, y);
	}

private:
	D2D_ALIGNMENT m_horz;
	D2D_ALIGNMENT m_vert;
};