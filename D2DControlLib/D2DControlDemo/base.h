// 继承 D2D1_RECT_F，添加常用矩形操作
class CD2DRectF : public D2D1_RECT_F
{
public:
	// 构造函数
	CD2DRectF() : D2D1_RECT_F(D2D1::RectF()) {}

	CD2DRectF(float left, float top, float right, float bottom)
		:D2D1_RECT_F(D2D1::RectF(left, top, right, bottom)){}

	explicit CD2DRectF(const D2D1_RECT_F& rect)
		: D2D1_RECT_F(rect) {}

	CD2DRectF& operator=(const D2D1_RECT_F& rect) {
		left = rect.left;
		top = rect.top;
		right = rect.right;
		bottom = rect.bottom;
		return *this;
	}

	// 转换为 D2D1_RECT_F（隐式转换）
	operator D2D1_RECT_F() const {
		return *this;  // 因为继承自 D2D1_RECT_F，直接返回即可
	}

	D2D1_POINT_2F Position() const {
		return D2D1::Point2F(left, top);
	}
	void SetPosition(const D2D1_POINT_2F& pt) {
		float w = Width();
		float h = Height();
		left = pt.x;
		top  = pt.y;
		right = left + w;
		bottom = top + h;
	}
	void SetPosition(float x, float y) {
		SetPosition(D2D1::Point2F(x, y));
	}

	// 获取宽高
	float Width() const  { return right - left; }
	float Height() const { return bottom - top; }

	// 新增：Size 相关
	D2D1_SIZE_F Size() const {
		return D2D1::SizeF(Width(), Height());
	}

	void SetSize(const D2D1_SIZE_F& size) {
		SetSize(size.width, size.height);
	}

	void SetSize(float width, float height) {
		right += width;
		bottom += height;
	}

	void SetWidth(float width) {
		right = left + width;
	}

	void SetHeight(float height) {
		bottom = top + height;
	}

	// 获取中心点
	float CenterX() const { return (left + right) * 0.5f; }
	float CenterY() const { return (top + bottom) * 0.5f; }

	D2D1_POINT_2F Center() const {
		return D2D1::Point2F((left + right) * 0.5f, (top + bottom) * 0.5f);
	}

	void SetCenter(const D2D1_POINT_2F& center) {
		float halfW = Width() * 0.5f;
		float halfH = Height() * 0.5f;
		left = center.x - halfW;
		right = center.x + halfW;
		top = center.y - halfH;
		bottom = center.y + halfH;
	}

	void SetCenter(float cx, float cy) {
		SetCenter(D2D1::Point2F(cx, cy));
	}

	// 获取左上/右下点坐标
	D2D1_POINT_2F TopLeft()     const { return D2D1::Point2F(left, top); }
	D2D1_POINT_2F BottomRight() const { return D2D1::Point2F(right, bottom); }


	// 判断矩形是否为空（宽或高 <= 0）
	bool IsEmpty() const {
		return (right <= left) || (bottom <= top);
	}

	// 判断点是否在矩形内
	bool Contains(float x, float y) const {
		return x >= left && x < right && y >= top && y < bottom;
	}
	bool Contains(const D2D1_POINT_2F& pt) const {
		return Contains(pt.x, pt.y);
	}

	// 判断矩形是否包含另一个矩形（完全包含）
	bool Contains(const CD2DRectF& rect) const {
		return (left <= rect.left && rect.right <= right &&
			top <= rect.top && rect.bottom <= bottom);
	}

	// 矩形相交测试
	bool Intersects(const CD2DRectF& rect) const {
		return !(rect.left >= right || rect.right <= left ||
			rect.top >= bottom || rect.bottom <= top);
	}

	// 求交集（返回新矩形，若无交集则返回空矩形）
	CD2DRectF Intersect(const CD2DRectF& rect) const {
		float l = max(left, rect.left);
		float t = max(top, rect.top);
		float r = min(right, rect.right);
		float b = min(bottom, rect.bottom);
		if (l >= r || t >= b) {
			return CD2DRectF(); // 空矩形
		}
		return CD2DRectF(l, t, r, b);
	}

	// 求并集（包围矩形）
	CD2DRectF Union(const CD2DRectF& rect) const {
		return CD2DRectF(min(left, rect.left), min(top, rect.top),
			max(right, rect.right), max(bottom, rect.bottom));
	}

	// 平移
	void Offset(float dx, float dy) {
		left += dx;
		right += dx;
		top += dy;
		bottom += dy;
	}

	// 缩放（以矩形中心为中心）
	void Scale(float sx, float sy) {
		float cx = CenterX();
		float cy = CenterY();
		float halfW = Width() * sx * 0.5f;
		float halfH = Height() * sy * 0.5f;
		left = cx - halfW;
		right = cx + halfW;
		top = cy - halfH;
		bottom = cy + halfH;
	}

	// 膨胀（向外扩展）
	void Inflate(float dx, float dy) {
		left -= dx;
		right += dx;
		top -= dy;
		bottom += dy;
	}

};
