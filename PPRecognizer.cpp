#include "PPRecognizer.h"
#include <iostream>
#include <vector>
#include <cmath>

#include "Vector2.h"

float PPRecognizer::PathLength(const std::vector<Vector2>& points)
{
    float d = 0;
    for (size_t i = 1; i < points.size(); ++i) {
        Vector2 prev = points[i - 1];
        Vector2 current = points[i];
        d += std::sqrt(std::pow(current.x - prev.x, 2) + std::pow(current.y - prev.y, 2));
    }
    return d;
}

std::vector<Vector2> PPRecognizer::ResampleBetweenPoints(const std::vector<Vector2>& points, size_t n)
{
    float I = PathLength(points) / (n - 1);
    float D = 0;
    std::vector<Vector2> v;
    Vector2 prev = points[0];
    std::vector<Vector2> modifiedPoints = points;

    for (size_t i = 1; i < modifiedPoints.size(); ++i) 
    {
        float d = modifiedPoints[i].Distance( modifiedPoints[i - 1]);

        if (D + d >= I) 
        {
            Vector2 q = modifiedPoints[i - 1] + Vector2((modifiedPoints[i].x - modifiedPoints[i - 1].x) * (I - D) / d,
                (modifiedPoints[i].y - modifiedPoints[i - 1].y) * (I - D) / d);
            Vector2 r = q - prev;
            r = Vector2(r.x / r.Distance({0, 0}), r.y / r.Distance({0, 0})); // Assuming r is a direction vector
            D = 0;
            prev = q;
            v.push_back(r);
            modifiedPoints.insert(modifiedPoints.begin() + i, q);
        }
        else 
        {
            D += d;
        }
    }
    return v;
}

std::pair<std::string, float> PPRecognizer::Recognize(const std::vector<Vector2>& points, int n, std::vector<GestureTemplate>& gestureTemplates)
{
    std::vector<Vector2> c = ResampleBetweenPoints(points, n);
    float similarity = -INFINITY;
    std::string bestTemplate;

    for (const auto& tpl : gestureTemplates)
    {
        std::vector<Vector2> t = tpl.points;
        float d = 0;
        for (size_t i = 0; i < n - 2; ++i)
        {
            d += t[i].x * c[i].x + t[i].y * c[i].y;
        }
        if (d > similarity)
        {
            similarity = d;
            bestTemplate = tpl.name;
        }
    }
    return { bestTemplate, similarity };
}