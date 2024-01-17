#pragma once
#include <string>
#include <vector>

struct XStrokeGestureTemplate
{
    std::string name;
    std::vector<int> values;

    XStrokeGestureTemplate(const std::string& n, const std::vector<int>& v)
    {
        name = n;
        values = v;
    }
    XStrokeGestureTemplate(const std::string& n, int v)
    {
        name = n;
        values.push_back(v);
    }

	auto operator+(const int v)
    {
        values.push_back(v);
    }
};
