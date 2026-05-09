
class CD2DSizePolicy
{
public:
	enum  Policy{
		Fixed = 0x0,        // 固定大小
		Minimum = 0x1,      // 最小大小
		Maximum = 0x2,      // 最大大小
		Preferred = 0x3,    // 首选大小 (默认)
		MinimumExpanding = 0x4, // 最小且可扩展 (已过时，但在源码中仍存在)
		Expanding = 0x5,    // 可扩展
		Ignored = 0x6       // 忽略 sizeHint
	};

	CD2DSizePolicy(Policy size)
	{
		m_policy = size;
	}

	CD2DSizePolicy() : m_policy(Fixed) {}

	// 提供接口来修改
	void SetPolicy(Policy p) { m_policy = p; }

	// 提供接口来获取
	Policy GetPolicy() const { return m_policy; }

	// 重载 == 运算符，支持直接比较对象（可选）
	bool operator==(Policy p) const { return m_policy == p; }

	Policy  m_policy;
};
