#pragma once
#include <vector>

#include "GestureTemplate.h"
#include "Vector2.h"

class PPRecognizer
{
public:
	PPRecognizer() = default;
	~PPRecognizer() = default;

	std::vector<Vector2> ResampleBetweenPoints(const std::vector<Vector2>& points, size_t n);
	std::pair<std::string, float> Recognize(const std::vector<Vector2>& points, int n, std::vector<GestureTemplate>& gestureTemplates);
private:
	float PathLength(const std::vector<Vector2>& points);
};

