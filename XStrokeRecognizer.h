#pragma once
#include <vector>
#include "Vector2.h"
#include "XStrokeGestureTemplate.h"


class XStrokeRecognizer
{
public:
	XStrokeRecognizer(Vector2 size);
	~XStrokeRecognizer();

	std::string DoRecognition(const std::vector<Vector2>& points);

	void SetName(const std::string& name) { m_TemplateName = name; };
	void ToggleSaveAsTemplate() { m_SaveAsTemplate = !m_SaveAsTemplate; };

	void SaveGestureTemplates();

private:
	int m_Rows{3};
	int m_Cols{3};
	int m_CellWidth;
	int m_CellHeight;

	std::vector<XStrokeGestureTemplate> m_GestureTemplates{};
	std::string m_TemplateName{ "test" };

	bool m_SaveAsTemplate{ false };

	std::string m_FileName{"templates/XStrokeTemplates.txt"};

	void ResizeGrid(Vector2 size);
	static Vector2 FindBounds(const std::vector<Vector2>& points);

	std::string Recognise(int p);
	int Normalize(const std::vector<Vector2>& points) const;
	void AddTemplate(int input);
	std::vector<XStrokeGestureTemplate> LoadGestureTemplates();
};
