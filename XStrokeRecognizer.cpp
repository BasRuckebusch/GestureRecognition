#include "XStrokeRecognizer.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

XStrokeRecognizer::XStrokeRecognizer(const Vector2 size)
{
	m_CellWidth = size.x / m_Cols;
	m_CellHeight = size.y / m_Rows;

    LoadGestureTemplates();
}

XStrokeRecognizer::~XStrokeRecognizer()
{
    SaveGestureTemplates();
}

void XStrokeRecognizer::ResizeGrid(const Vector2 size)
{
    m_CellWidth = size.x / m_Cols;
    m_CellHeight = size.y / m_Rows;
}

Vector2 XStrokeRecognizer::FindBounds(const std::vector<Vector2>& points)
{
    if (points.empty()) 
        return Vector2{ 0.0f, 0.0f };

    Vector2 lowest = points[0];
    Vector2 highest = points[0];

    for (const Vector2& point : points)
    {
        if (point.x < lowest.x)
            lowest.x = point.x;
        if (point.y < lowest.y)
            lowest.y = point.y; 
        if (point.x > highest.x)
            highest.x = point.x;
        if (point.y > highest.y) 
            highest.y = point.y;
    }

    return Vector2{ 0.0f, 0.0f };
}

std::string XStrokeRecognizer::DoRecognition(const std::vector<Vector2>& points)
{
	const int p{ Normalize(points) };
	if (!m_SaveAsTemplate)
        return Recognise(p);

    AddTemplate(p);
    return "Gestured saved as: " + m_TemplateName + "\n";
}

std::string XStrokeRecognizer::Recognise(const int p)
{
    for (auto& gestureTemplate : m_GestureTemplates)
    {
	    for (const auto v : gestureTemplate.values)
	    {
		    if (v == p)
		    {
                return gestureTemplate.name;
		    }
	    }
    }
    return "Gesture not found!";
}

int XStrokeRecognizer::Normalize(const std::vector<Vector2>& points) const
{
    int result{ 0 };

    for (const auto& point : points)
    {
        const int col = static_cast<int>(point.x / m_CellWidth);
        const int row = static_cast<int>(point.y / m_CellHeight);

        const int gridPosition = row * m_Cols + col + 1;
        if (result == 0)
        {
            result = gridPosition;
        }
        else
        {
            const int lastDigit = result % 10;
            if (lastDigit != gridPosition)
            {
                result = (result * 10) + gridPosition;
            }
        }
    }

    return result;
}

void XStrokeRecognizer::AddTemplate(const int input)
{
    int found{ -1 };
    if (!m_GestureTemplates.empty())
    {
        for (int i = 0; i < m_GestureTemplates.size(); ++i)
        {
            if (m_GestureTemplates[i].name == m_TemplateName)
            {
                found = i;
                break;
            }
        }
    }

    if (found > -1)
    {
        m_GestureTemplates[found]  + input;
    }
    else
    {
	    const XStrokeGestureTemplate newTemplate(m_TemplateName, input);
        m_GestureTemplates.push_back(newTemplate);
    }
}

void XStrokeRecognizer::SaveGestureTemplates()
{
    std::ofstream outFile(m_FileName);

    if (outFile.is_open()) 
    {
        for (const auto& gesture : m_GestureTemplates) 
        {
            outFile << gesture.name << " ";
            for (const auto& value : gesture.values) 
            {
                outFile << value << " ";
            }
            outFile << std::endl;
        }
        outFile.close();
    }
    else 
        std::cerr << "Unable to open file: " << m_FileName << std::endl;
}

std::vector<XStrokeGestureTemplate> XStrokeRecognizer::LoadGestureTemplates()
{
    std::vector<XStrokeGestureTemplate> loadedGestures;
    std::ifstream inFile(m_FileName);

    if (inFile.is_open())
    {
        std::string line;
        while (std::getline(inFile, line)) 
        {
            std::istringstream iss(line);
            std::string gestureName;
            iss >> gestureName;

            std::vector<int> values;
            int val;
            while (iss >> val) 
            {
                values.push_back(val);
            }

            loadedGestures.emplace_back(gestureName, values);
            m_GestureTemplates.emplace_back(gestureName, values);
        }
        inFile.close();
    }
    else
    {
        // Handle file opening failure
        std::cerr << "Unable to open file: " << m_FileName << std::endl;
    }
    return loadedGestures;
}