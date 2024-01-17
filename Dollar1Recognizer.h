#pragma once
#include <string>
#include <vector>

#include "GestureTemplate.h"
#include "Vector2.h"

class Dollar1Recognizer
{
public:
    Dollar1Recognizer();
    ~Dollar1Recognizer();

    std::vector<Vector2> Normalize(const std::vector<Vector2>& points, int targetPointCount) const;
    std::pair<std::string, float> DoRecognition(const std::vector<Vector2>& points, int n, std::vector<GestureTemplate>& gestureTemplates) const;

private:
    static std::vector<Vector2> Resample(std::vector<Vector2>& points, int targetPointCount);
    static float PathLength(const std::vector<Vector2>& path);
    static Vector2 CalculateCentroid(const std::vector<Vector2>& points);
    std::vector<Vector2> RotateToZero(std::vector<Vector2>& points) const;
    static std::vector<Vector2> RotatePoints(const std::vector<Vector2>& points, float angle);
    static std::vector<Vector2> ScaleToSquare(const std::vector<Vector2>& points, float targetSize);
    static Vector2 CalculateBoundingBox(const std::vector<Vector2>& points);
    static std::vector<Vector2> TranslateToOrigin(const std::vector<Vector2>& points);
    std::pair<std::string, float>  Recognize(std::vector<Vector2>& inputPoints, std::vector<GestureTemplate>& gestureTemplates) const;
    static float DistanceAtBestAngle(std::vector<Vector2>& inputPoints, std::vector<Vector2>& templatePoints, float angleA, float angleB, float angleDelta);
    static float DistanceAtAngle(const std::vector<Vector2>& inputPoints, std::vector<Vector2>& templatePoints, float theta);
    static float PathDistance(std::vector<Vector2>& A, std::vector<Vector2>& B);
};