#pragma once
#include <vector>

#include "GestureTemplate.h"
#include "PPRecognizer.h"

class PPGestureManager
{
public:
	PPGestureManager();
	~PPGestureManager();

	void Process(const std::vector<Vector2>& drawpoints);

	void SetName(const std::string name) { m_TemplateName = name; };
	void ToggleSaveAsTemplate() { m_SaveAsTemplate = !m_SaveAsTemplate; };

	void SaveTemplates() const;
	void LoadTemplates();

private:
	bool m_SaveAsTemplate{ false };
	PPRecognizer m_PP{};
	int m_NormalizePointSize{ 128 };
	std::vector<GestureTemplate> m_GestureTemplates{};
	std::string m_TemplateName{ "test" };
	std::string m_FileName{ "templates/PPTemplates.txt" };
};
