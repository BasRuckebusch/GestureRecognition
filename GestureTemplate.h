#pragma once
#include <string>
#include <utility>
#include <vector>

#include "Vector2.h"

struct GestureTemplate
{
    std::string name;
    std::vector<Vector2> points;

    GestureTemplate(std::string templateName, const std::vector<Vector2>& preparePoints)
	{
        name = std::move(templateName);
        points = preparePoints;
    }
    GestureTemplate()
	{
        name = "";
        points = std::vector<Vector2>{};
    }
};
