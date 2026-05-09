#pragma once

#include <d2d1.h>
#include <cmath>

// 几何计算工具类
class D2DGeometryMath
{
public:
    // 根据圆心计算圆上的坐标
    // 参数: center-圆心坐标, radius-半径, angle-角度(弧度制)
    // 返回值:圆上对应角度的点的坐标
    static D2D1_POINT_2F CalculatePointOnCircle(const D2D1_POINT_2F& center, float radius, float angle);

    // 将角度从角度制转换为弧度制
    static float DegreesToRadians(float degrees);

    // 将角度从弧度制转换为角度制
    static float RadiansToDegrees(float radians);

    // 用途 ：计算两点之间的欧几里得距离。
    // 参数: point1-点1坐标, point2-点2坐标
    // 返回值:两点之间的距离
    static float CalculateDistance(const D2D1_POINT_2F& point1, const D2D1_POINT_2F& point2);

	// 计算两点直线的中点坐标
	// 参数: point1-第一个点, point2-第二个点
	// 返回: 两点的中点坐标
	static D2D1_POINT_2F CalculateLineMidPoint(const D2D1_POINT_2F& point1, const D2D1_POINT_2F& point2);
    
    // 计算两点之间的线性插值点
    // 参数: pointA-起点A, pointB-终点B, t-插值参数(0.0f到1.0f)
    // 返回值:根据参数t在线段AB上的插值点
    static D2D1_POINT_2F CalculateLinearInterpolationPoint(const D2D1_POINT_2F& pointA, const D2D1_POINT_2F& pointB, float t);
    
    // 用途 ：计算点到直线的垂直距离
    // 参数: point-点坐标, lineStart-直线起点坐标, lineEnd-直线终点坐标
    // 返回值:点到直线的垂直距离
    static float CalculateDistancePointToLine(
        const D2D1_POINT_2F& point,
        const D2D1_POINT_2F& lineStart,
        const D2D1_POINT_2F& lineEnd);

    // 计算向量的长度
    static float CalculateVectorLength(const D2D1_POINT_2F& vector);

    // 计算归一化向量
    static D2D1_POINT_2F NormalizeVector(const D2D1_POINT_2F& vector);

    // 计算两个向量的点积
    static float CalculateDotProduct(const D2D1_POINT_2F& vector1, const D2D1_POINT_2F& vector2);

	// 根据二次贝塞尔曲线的三个点和参数t计算对应的点
	// 参数: p0-起点, p1-控制点, p2-终点, t (0.0f 到 1.0f)
	// 返回: t时刻曲线上的点
	static D2D1_POINT_2F CalculateQuadraticBezierPoint(const D2D1_POINT_2F& p0, const D2D1_POINT_2F& p1, const D2D1_POINT_2F& p2, float t);
    
	// 计算对称二次贝塞尔曲线的控制点
    // 参数: startPoint-起点, endPoint-终点, t-参数值(0到1之间), pointOnCurve-曲线上对应 t 的点
    // 返回值:对称二次贝塞尔曲线的控制点
    static D2D1_POINT_2F CalculateQuadraticBezierControlPoint(const D2D1_POINT_2F& startPoint, const D2D1_POINT_2F& endPoint, float t, const D2D1_POINT_2F& pointOnCurve); 

    // 计算矩形内的点
    // 参数: rect-矩形, ratioX-水平比例(0到1之间), ratioY-垂直比例(0到1之间)
    // 返回值:矩形内对应比例的点坐标
    static D2D1_POINT_2F CalculatePointInRect(const D2D1_RECT_F& rect, float ratioX, float ratioY);

	// 检查点是否在矩形内
	static bool IsPointInRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect);

    // 检查点是否在椭圆内
    static bool IsPointInEllipse(const D2D1_POINT_2F& point, const D2D1_ELLIPSE& ellipse);

    // 检查点是否在三角形内
    static bool IsPointInTriangle(const D2D1_POINT_2F& point, const D2D1_POINT_2F& vertex1, 
                                 const D2D1_POINT_2F& vertex2, const D2D1_POINT_2F& vertex3);

    // 检查点是否在矩形的边上
    static bool IsPointOnRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect, float tolerance = 1.0f);

    // 检查点是否在椭圆的边上
    static bool IsPointOnEllipse(const D2D1_POINT_2F& point, const D2D1_ELLIPSE& ellipse, float tolerance = 1.0f);

    // 检查点是否在三角形的边上
    static bool IsPointOnTriangle(const D2D1_POINT_2F& point, const D2D1_POINT_2F& vertex1, 
                                 const D2D1_POINT_2F& vertex2, const D2D1_POINT_2F& vertex3, 
                                 float tolerance = 1.0f);

    // 检查点是否在线段上
    static bool IsPointOnLineSegment(const D2D1_POINT_2F& point, const D2D1_POINT_2F& lineStart, 
                                   const D2D1_POINT_2F& lineEnd, float tolerance = 1.0f);
    
    
	//矩形角度相交点
	static bool RectFDegressIntersectPoints(float degress, D2D1_RECT_F rect, D2D1_POINT_2F& start, D2D1_POINT_2F& end);

};