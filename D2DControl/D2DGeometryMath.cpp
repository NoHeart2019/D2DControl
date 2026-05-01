#include "stdafx.h"
#include "D2DGeometryMath.h"

// 根据圆心计算圆上的坐标
/*
	### 坐标系说明
	在Direct2D中，坐标系采用如下约定：

	- 向右为X轴正方向
	- 向下为Y轴正方向
	### 角度说明
	- 参数方程中的角度 θ 使用 弧度制
	- 如果您有角度制的角度值，需要先转换为弧度：
	### 角度与位置的关系
	- 0 弧度（0度）：点位于圆心的正右方
	- π/2 弧度（90度）：点位于圆心的正下方
	- π 弧度（180度）：点位于圆心的正左方
	- 3π/2 弧度（270度）：点位于圆心的正上方
*/
D2D1_POINT_2F D2DGeometryMath::CalculatePointOnCircle(const D2D1_POINT_2F& center, float radius, float angle)
{
    // 使用圆的参数方程计算坐标
    // x = center.x + radius * cos(angle)
    // y = center.y + radius * sin(angle)
    D2D1_POINT_2F point;
    point.x = center.x + radius * cosf(angle);
    point.y = center.y + radius * sinf(angle);
    return point;
}

// 将角度从角度制转换为弧度制
float D2DGeometryMath::DegreesToRadians(float degrees)
{
    return degrees * (float)M_PI / 180.0f;
}

// 将角度从弧度制转换为角度制
float D2DGeometryMath::RadiansToDegrees(float radians)
{
    return radians * 180.0f / (float)M_PI;
}

// 计算两点之间的距离
float D2DGeometryMath::CalculateDistance(const D2D1_POINT_2F& point1, const D2D1_POINT_2F& point2)
{
    float dx = point2.x - point1.x;
    float dy = point2.y - point1.y;
    return sqrtf(dx * dx + dy * dy);
}

// 计算两点之间连线的中点
D2D1_POINT_2F D2DGeometryMath::CalculateLineMidPoint(const D2D1_POINT_2F& point1, const D2D1_POINT_2F& point2)
{
    D2D1_POINT_2F midpoint;
    midpoint.x = (point1.x + point2.x) / 2.0f;
    midpoint.y = (point1.y + point2.y) / 2.0f;
    return midpoint;
}

// 计算矩形内的点
// 参数: rect-矩形, ratioX-水平比例(0到1之间), ratioY-垂直比例(0到1之间)
// 返回值:矩形内对应比例的点坐标
D2D1_POINT_2F D2DGeometryMath::CalculatePointInRect(const D2D1_RECT_F& rect, float ratioX, float ratioY)
{
    D2D1_POINT_2F point;
    point.x = rect.left + ratioX * (rect.right - rect.left);
    point.y = rect.top + ratioY * (rect.bottom - rect.top);
    return point;
}

// 计算点到直线的距离
float D2DGeometryMath::CalculateDistancePointToLine(
    const D2D1_POINT_2F& point,
    const D2D1_POINT_2F& lineStart,
    const D2D1_POINT_2F& lineEnd)
{
    // 计算向量
    D2D1_POINT_2F A = D2D1::Point2F(point.x - lineStart.x, point.y - lineStart.y);
    D2D1_POINT_2F B = D2D1::Point2F(lineEnd.x - lineStart.x, lineEnd.y - lineStart.y);
    
    // 计算向量的叉积
    float crossProduct = A.x * B.y - A.y * B.x;
    
    // 计算向量B的长度
    float lengthB = CalculateVectorLength(B);
    
    // 返回距离
    return fabsf(crossProduct) / lengthB;
}

// 计算向量的长度
float D2DGeometryMath::CalculateVectorLength(const D2D1_POINT_2F& vector)
{
    return sqrtf(vector.x * vector.x + vector.y * vector.y);
}

// 计算归一化向量
D2D1_POINT_2F D2DGeometryMath::NormalizeVector(const D2D1_POINT_2F& vector)
{
    float length = CalculateVectorLength(vector);
    if (length > 0.0001f) // 避免除以零
    {
        return D2D1::Point2F(vector.x / length, vector.y / length);
    }
    return D2D1::Point2F(0.0f, 0.0f);
}

// 计算两个向量的点积
float D2DGeometryMath::CalculateDotProduct(const D2D1_POINT_2F& vector1, const D2D1_POINT_2F& vector2)
{
    return vector1.x * vector2.x + vector1.y * vector2.y;
}

#include <d2d1.h>

// 根据二次贝塞尔曲线的三个点和参数t计算对应的点
D2D1_POINT_2F CalculateQuadraticBezierPoint(const D2D1_POINT_2F& p0, 
	const D2D1_POINT_2F& p1, 
	const D2D1_POINT_2F& p2, 
	float t)
{
	// 二次贝塞尔曲线公式: B(t) = (1-t)² * P0 + 2(1-t)t * P1 + t² * P2
	float oneMinusT = 1.0f - t;
	float oneMinusTSquared = oneMinusT * oneMinusT;
	float tSquared = t * t;

	D2D1_POINT_2F point;
	point.x = oneMinusTSquared * p0.x + 2.0f * oneMinusT * t * p1.x + tSquared * p2.x;
	point.y = oneMinusTSquared * p0.y + 2.0f * oneMinusT * t * p1.y + tSquared * p2.y;

	return point;
}

D2D1_POINT_2F D2DGeometryMath::CalculateQuadraticBezierControlPoint(
    const D2D1_POINT_2F& startPoint,
    const D2D1_POINT_2F& endPoint,
    float t,
    const D2D1_POINT_2F& pointOnCurve)
{
    if (t <= 0.0f || t >= 1.0f)
    {
        return D2D1::Point2F( 0.0f, 0.0f);
    }

    float t2 = t * t;
    float one_minus_t = 1.0f - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    float denominator = 2.0f * t * one_minus_t;

    D2D1_POINT_2F controlPoint;
    controlPoint.x = (pointOnCurve.x - one_minus_t2 * startPoint.x - t2 * endPoint.x) / denominator;
    controlPoint.y = (pointOnCurve.y - one_minus_t2 * startPoint.y - t2 * endPoint.y) / denominator;

    return controlPoint;
}

// 检查点是否在矩形内
bool D2DGeometryMath::IsPointInRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect)
{
	return point.x >= rect.left && point.x <= rect.right &&
		point.y >= rect.top && point.y <= rect.bottom;
}

// 计算两点直线的中点坐标
D2D1_POINT_2F CalculateLineMidPoint(D2D1_POINT_2F point1, D2D1_POINT_2F point2)
{
	D2D1_POINT_2F midPoint;
	midPoint.x = (point1.x + point2.x) * 0.5f;
	midPoint.y = (point1.y + point2.y) * 0.5f;
	return midPoint;
}

// 检查点是否在椭圆内
bool D2DGeometryMath::IsPointInEllipse(const D2D1_POINT_2F& point, const D2D1_ELLIPSE& ellipse)
{
    // 转换为椭圆坐标系
    float dx = (point.x - ellipse.point.x) / ellipse.radiusX;
    float dy = (point.y - ellipse.point.y) / ellipse.radiusY;
    
    // 椭圆方程：x²/a² + y²/b² <= 1
    return (dx * dx + dy * dy) <= 1.0f;
}

// 检查点是否在三角形内
bool D2DGeometryMath::IsPointInTriangle(const D2D1_POINT_2F& point, const D2D1_POINT_2F& vertex1, 
                                             const D2D1_POINT_2F& vertex2, const D2D1_POINT_2F& vertex3)
{
    // 使用重心坐标法
    float denom = (vertex2.y - vertex3.y) * (vertex1.x - vertex3.x) + (vertex3.x - vertex2.x) * (vertex1.y - vertex3.y);
    
    // 计算重心坐标u
    float u = ((vertex2.y - vertex3.y) * (point.x - vertex3.x) + (vertex3.x - vertex2.x) * (point.y - vertex3.y)) / denom;
    
    // 计算重心坐标v
    float v = ((vertex3.y - vertex1.y) * (point.x - vertex3.x) + (vertex1.x - vertex3.x) * (point.y - vertex3.y)) / denom;
    
    // 计算重心坐标w
    float w = 1.0f - u - v;
    
    // 点在三角形内当且仅当u >= 0, v >= 0, w >= 0
    return (u >= 0.0f) && (v >= 0.0f) && (w >= 0.0f);
}

// 检查点是否在矩形的边上
bool D2DGeometryMath::IsPointOnRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect, float tolerance)
{
    // 检查点是否在矩形的任何一条边上
    bool onLeftEdge = (fabs(point.x - rect.left) <= tolerance) && (point.y >= rect.top - tolerance) && (point.y <= rect.bottom + tolerance);
    bool onRightEdge = (fabs(point.x - rect.right) <= tolerance) && (point.y >= rect.top - tolerance) && (point.y <= rect.bottom + tolerance);
    bool onTopEdge = (fabs(point.y - rect.top) <= tolerance) && (point.x >= rect.left - tolerance) && (point.x <= rect.right + tolerance);
    bool onBottomEdge = (fabs(point.y - rect.bottom) <= tolerance) && (point.x >= rect.left - tolerance) && (point.x <= rect.right + tolerance);
    
    return onLeftEdge || onRightEdge || onTopEdge || onBottomEdge;
}

// 检查点是否在椭圆的边上
bool D2DGeometryMath::IsPointOnEllipse(const D2D1_POINT_2F& point, const D2D1_ELLIPSE& ellipse, float tolerance)
{
    // 转换为椭圆坐标系
    float dx = (point.x - ellipse.point.x) / ellipse.radiusX;
    float dy = (point.y - ellipse.point.y) / ellipse.radiusY;
    
    // 计算到椭圆的距离
    float distance = dx * dx + dy * dy;
    
    // 检查是否在椭圆的边上（考虑容差）
    return fabs(distance - 1.0f) <= (tolerance * 2.0f / (min(ellipse.radiusX, ellipse.radiusY) * min(ellipse.radiusX, ellipse.radiusY)));
}

// 检查点是否在三角形的边上
bool D2DGeometryMath::IsPointOnTriangle(const D2D1_POINT_2F& point, const D2D1_POINT_2F& vertex1, 
                                             const D2D1_POINT_2F& vertex2, const D2D1_POINT_2F& vertex3, 
                                             float tolerance)
{
    // 检查点是否在三角形的任何一条边上
    return IsPointOnLineSegment(point, vertex1, vertex2, tolerance) ||
           IsPointOnLineSegment(point, vertex2, vertex3, tolerance) ||
           IsPointOnLineSegment(point, vertex3, vertex1, tolerance);
}

// 检查点是否在线段上
bool D2DGeometryMath::IsPointOnLineSegment(const D2D1_POINT_2F& point, const D2D1_POINT_2F& lineStart, 
                                               const D2D1_POINT_2F& lineEnd, float tolerance)
{
    // 计算点到线段的距离
    float distance = CalculateDistancePointToLine(point, lineStart, lineEnd);
    
    // 检查距离是否小于容差
    if (distance > tolerance)
    {
        return false;
    }
    
    // 检查点是否在线段的延长线上
    float dotProduct = CalculateDotProduct(D2D1::Point2F(point.x - lineStart.x, point.y - lineStart.y),
                                          D2D1::Point2F(point.x - lineEnd.x, point.y - lineEnd.y));
    
    // 点在线段上的条件是：距离小于容差，并且向量点积小于等于0
    return dotProduct <= tolerance * tolerance;
}


bool RectFDegressIntersectPoints(float degress, D2D1_RECT_F rect, D2D1_POINT_2F& start, D2D1_POINT_2F& end)
{
	// === 计算起点和终点 ===
	float centerX = (rect.left + rect.right) * 0.5f;
	float centerY = (rect.top + rect.bottom) * 0.5f;

	// CSS angle: 0° = up (negative Y), 90° = right (positive X)
	// 转换为数学角度（从 X 轴正方向逆时针）
	float cssToMathAngle = 90.0f - degress; // 因为 CSS 0° 是 Y 负方向
	float rad = cssToMathAngle * static_cast<float>(M_PI) / 180.0f;

	float dirX = cosf(rad);
	float dirY = sinf(rad);

	// 辅助函数：从中心沿方向 (dx, dy) 射线，求与矩形边界的交点
	auto RayIntersectRect = [&](float dx, float dy) -> D2D1_POINT_2F {
		float t1 = (dx > 0) ? (rect.right - centerX) / dx : (dx < 0) ? (rect.left - centerX) / dx : FLT_MAX;
		float t2 = (dy > 0) ? (rect.bottom - centerY) / dy : (dy < 0) ? (rect.top - centerY) / dy : FLT_MAX;
		float t = min(max(0, t1), max(0, t2)); // 取最小正 t
		if (t == FLT_MAX) t = 0; // 防止除零
		return D2D1::Point2F(centerX + dx * t, centerY + dy * t);
	};

	end   = RayIntersectRect(dirX, dirY);     // 沿 angle 方向
	start = RayIntersectRect(-dirX, -dirY);   // 沿反方向

	return true;
}

