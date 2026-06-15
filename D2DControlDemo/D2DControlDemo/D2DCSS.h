#pragma once


/*
用 Direct2D 实现一组常见 CSS 功能，适用于简单 UI 控件.

边框样式（四条边，线型，圆角）
背景（纯色，线性渐变，径向渐变）
状态 悬浮，按下，拖拽（不同状态对应不同的背景色）
变换： 仅支持平移

编码要求 ：C++, 仅使用Atl库，不准使用std库。函数名尽量符合Css的名称
*/

typedef D2D_RECT_F CMargin;
typedef D2D_RECT_F CPadding;

/*
	
*/
class CD2DControlUI {
public:
	CD2DControlUI();
	virtual ~CD2DControlUI();
	
protected:
	
	enum ELineStyle
	{
		None,
		Solid,
		Dashed,
		Dotted,
		Double,
		// ... 可扩展
	};

	enum  EState {
		Normal,
		Hover,
		Pressed,
		Disabled,
		Dragging,   // 可扩展
		Focused, 
		Count=6       // 方便定义数组大小
	};

	struct CBorderEdge
	{
		float        m_width;
		ELineStyle   m_style;
		D2D1_COLOR_F m_color;
		float        dashLength;   // dashed/dotted 时有效
		float        gapLength;    // dashed 时有效
	};

	struct CRadius
	{
		float m_topLeft;
		float m_topRight;
		float m_bottomRight;
		float m_bottomLeft;
	};

	struct CBorder
	{
		CBorderEdge  m_top;
		CBorderEdge  m_right;
		CBorderEdge  m_bottom;
		CBorderEdge  m_left;
	};

	struct CBackground
	{
	public:

		enum EType
		{
			None,
			Solid,
			LinearGradient,
			RadialGradient   // 可扩展
		};

		struct CSolidColor
		{
			D2D1_COLOR_F color;
		};

		struct CGradientStop
		{
			float position;         // 0.0 ~ 1.0
			D2D1_COLOR_F color;
		};

		struct CLinearGradient
		{
			D2D1_POINT_2F startPoint;
			D2D1_POINT_2F endPoint;
			D2D1_GRADIENT_STOP		m_backgroundStops[10];
			int						m_backgroundStopsCount;
		};

		struct CRadialGradient
		{
			D2D1_POINT_2F center;
			D2D1_POINT_2F gradientOriginOffset;
			float radiusX;
			float radiusY;
			D2D1_GRADIENT_STOP		m_backgroundStops[10];
			int						m_backgroundStopsCount;
		};

		EType					m_type;
		CSolidColor				m_solid;
		CLinearGradient			m_linear;
		CRadialGradient			m_radial;
	};

	struct CTransform
	{
		float m_translateX;
		float m_translateY;
	};

	CRadius				m_radius;    //border-radius
	CBorder				m_border;    //border
	CMargin				m_margin;    //margin
	CPadding			m_padding;   //padding
	CBackground			m_background[6];//background  
	CTransform			m_transform;    //transform 
	EState				m_status;    //hover pressed 

	D2D1_RECT_F			m_rectangle; //矩形包含margin。
};
