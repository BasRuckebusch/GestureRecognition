#include "DollarGestureManager.h"

#include <iostream>
#include <filesystem>
#include <fstream>

DollarGestureManager::DollarGestureManager()
{
    LoadTemplates();
}

DollarGestureManager::~DollarGestureManager() = default;

void DollarGestureManager::Process(const std::vector<Vector2>& drawpoints)
{
	if (drawpoints.empty())
		return;

	const std::vector<Vector2> processedPoints = m_D1.Normalize(drawpoints, m_NormalizePointSize);

	if (!m_SaveAsTemplate)
	{
		const std::pair<std::string, float> result = m_D1.DoRecognition(drawpoints, m_NormalizePointSize, m_GestureTemplates);
		std::cout << "Best Template: " + std::get<0>(result) + ", Score: " + std::to_string(std::get<1>(result)) + "\n";
		return;
	}
	
	const GestureTemplate gt{m_TemplateName, processedPoints};
	m_GestureTemplates.push_back(gt);
    std::cout << "Saved template as: " + m_TemplateName + "\n";
}

void DollarGestureManager::SaveTemplates() const
{
    std::ofstream outFile(m_FileName);

    if (outFile.is_open())
    {
        for (const auto& gesture : m_GestureTemplates)
        {
            outFile << gesture.name << " ";
            for (const auto& value : gesture.points)
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

void DollarGestureManager::LoadTemplates()
{
    std::vector<GestureTemplate> loadedGestures;
    std::ifstream inFile(m_FileName);

    if (inFile.is_open())
    {
        std::string line;
        while (std::getline(inFile, line))
        {
            std::istringstream iss(line);
            std::string gestureName;
            iss >> gestureName;

            std::vector<Vector2> points;
            Vector2 val;
            while (iss >> val)
            {
                points.push_back(val);
            }

            loadedGestures.emplace_back(gestureName, points);
            m_GestureTemplates.emplace_back(gestureName, points);
        }
        inFile.close();
    }
    else
    {
        // Handle file opening failure
        std::cerr << "Unable to open file: " << m_FileName << std::endl;
    }
}
