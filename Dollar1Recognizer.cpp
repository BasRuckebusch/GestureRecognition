#include "Dollar1Recognizer.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>

#define M_PI 3.14159265358979323846

Dollar1Recognizer::Dollar1Recognizer()
= default;

Dollar1Recognizer::~Dollar1Recognizer()
= default;

std::vector<Vector2> Dollar1Recognizer::Normalize(const std::vector<Vector2>& points, int targetPointCount) const
{
	std::vector<Vector2> copyPoints = points;
	std::vector<Vector2> resampledPoints = Resample(copyPoints, targetPointCount);
	const std::vector<Vector2> rotatedPoints = RotateToZero(resampledPoints);
	const std::vector<Vector2> scaledPoints = ScaleToSquare(rotatedPoints, 100);
	std::vector<Vector2> translatedToOrigin = TranslateToOrigin(scaledPoints);

	return translatedToOrigin;
}

std::pair<std::string, float> Dollar1Recognizer::DoRecognition(const std::vector<Vector2>& points, const int n, std::vector<GestureTemplate>& gestureTemplates) const
{
	std::vector<Vector2> preparedPoints = Normalize(points, n);
	return Recognize(preparedPoints, gestureTemplates);
}

// Step 1 
std::vector<Vector2> Dollar1Recognizer::Resample(std::vector<Vector2>& points, const int targetPointCount)
{
	const float interval = PathLength(points) / (targetPointCount - 1);
	float distanceSoFar = 0.0f;
	std::vector<Vector2> newPointsList;

	for (size_t i = 1; i < points.size(); ++i)
	{
		const float segmentDistance = points[i - 1].Distance(points[i]);

		if (distanceSoFar + segmentDistance >= interval) 
		{
			const float ratio = (interval - distanceSoFar) / segmentDistance;
			const float newX = points[i - 1].x + ratio * (points[i].x - points[i - 1].x);
			const float newY = points[i - 1].y + ratio * (points[i].y - points[i - 1].y);
			Vector2 newPoint(newX, newY);

			newPointsList.push_back(newPoint);
			points.insert(points.begin() + i, newPoint);
			distanceSoFar = 0.0f;
		}
		else 
		{
			distanceSoFar += segmentDistance;
		}
	}

	return newPointsList;
}

float Dollar1Recognizer::PathLength(const std::vector<Vector2>& path)
{
	float totalDistance = 0.0f;
	for (size_t i = 1; i < path.size(); ++i) 
	{
		totalDistance += path[i - 1].Distance(path[i]);
	}
	return totalDistance;
}

Vector2 Dollar1Recognizer::CalculateCentroid(const std::vector<Vector2>& points)
{
	Vector2 centroid = ZeroVector2;

	for (const auto& p : points) 
	{
		centroid += p;
	}

	return centroid / static_cast<float>(points.size());
}

std::vector<Vector2> Dollar1Recognizer::RotateToZero(std::vector<Vector2>& points) const
{
	// Calculate the centroid of the points
	const Vector2 centroid = CalculateCentroid(points);

	// Calculate the angle between the centroid and the first point
	const float angleToRotate = atan2(centroid.y - points[0].y, centroid.x - points[0].x);

	// Rotate the points by the negative of the calculated angle
	return RotatePoints(points, -angleToRotate);
}

std::vector<Vector2> Dollar1Recognizer::RotatePoints(const std::vector<Vector2>& points, float angle)
{
	std::vector<Vector2> newPoints(points.size());

	// Calculate the centroid of the points
	const Vector2 centroid = CalculateCentroid(points);

	for (int i = 0; i < points.size(); i++)
	{
		// Apply rotation transformation to each point
		const float qx = (points[i].x - centroid.x) * cos(angle) - (points[i].y - centroid.y) * sin(angle) + centroid.x;
		const float qy = (points[i].x - centroid.x) * sin(angle) + (points[i].y - centroid.y) * cos(angle) + centroid.y;
		newPoints[i] = Vector2(qx, qy);
	}

	return newPoints;
}

std::vector<Vector2> Dollar1Recognizer::ScaleToSquare(const std::vector<Vector2>& points, float targetSize)
{
	const Vector2 boundingBox = CalculateBoundingBox(points);
	std::vector<Vector2> newPoints(points.size());

	for (int i = 0; i < points.size(); i++)
	{
		// Scale each point based on the bounding box dimensions
		const float scaledX = points[i].x * (targetSize / boundingBox.x);
		const float scaledY = points[i].y * (targetSize / boundingBox.y);
		newPoints[i] = Vector2(scaledX, scaledY);
	}

	return newPoints;
}

Vector2 Dollar1Recognizer::CalculateBoundingBox(const std::vector<Vector2>& points)
{
	float minX = std::numeric_limits<float>::infinity();
	float minY = std::numeric_limits<float>::infinity();
	float maxX = -std::numeric_limits<float>::infinity();
	float maxY = -std::numeric_limits<float>::infinity();

	for (const auto& p : points) 
	{
		if (p.x < minX) minX = p.x;
		if (p.x > maxX) maxX = p.x;
		if (p.y < minY) minY = p.y;
		if (p.y > maxY) maxY = p.y;
	}

	return { maxX - minX, maxY - minY };
}

// Step 4
std::vector<Vector2> Dollar1Recognizer::TranslateToOrigin(const std::vector<Vector2>& points)
{
	const Vector2 centroid = CalculateCentroid(points);
	std::vector<Vector2> newPoints(points.size());

	for (int i = 0; i < points.size(); i++)
	{
		// Translate each point by subtracting the centroid
		newPoints[i] = points[i] - centroid;
	}
	return newPoints;
}

std::pair<std::string, float> Dollar1Recognizer::Recognize(std::vector<Vector2>& inputPoints, std::vector<GestureTemplate>& gestureTemplates) const
{
	float bestDistance = INFINITY;
	std::vector<Vector2> bestMatchedTemplate{};
	float bestScore = 0;
	std::string bestTemplateName = "None";

	for (auto& t : gestureTemplates) 
	{
		float distance = DistanceAtBestAngle(inputPoints, t.points, -M_PI, M_PI, 2 * M_PI / 180);

		if (distance < bestDistance)
		{
			bestDistance = distance;
			bestMatchedTemplate = t.points;
			bestScore = 1 - bestDistance / (0.5f * static_cast<float>(sqrt(bestMatchedTemplate.size() * bestMatchedTemplate.size() + bestMatchedTemplate.size() * bestMatchedTemplate.size())));
			bestTemplateName = t.name;
		}
	}

	return std::make_pair(bestTemplateName, bestScore);
}

float Dollar1Recognizer::DistanceAtBestAngle(std::vector<Vector2>& inputPoints, std::vector<Vector2>& templatePoints, float angleA, float angleB, float angleDelta)
{
	const float phi = .5f * (-1 + sqrt(5));
	float x1 = phi * angleA + (1 - phi) * angleB;
	float f1 = DistanceAtAngle(inputPoints, templatePoints, x1);
	float x2 = (1 - phi) * angleA + phi * angleB;
	float f2 = DistanceAtAngle(inputPoints, templatePoints, x2);

	while (abs(angleB - angleA) > angleDelta)
	{
		if (f1 < f2)
		{
			angleB = x2;
			x2 = x1;
			f2 = f1;
			x1 = phi * angleA + (1 - phi) * angleB;
			f1 = DistanceAtAngle(inputPoints, templatePoints, x1);
		}
		else
		{
			angleA = x1;
			x1 = x2;
			f1 = f2;
			x2 = (1 - phi) * angleA + phi * angleB;
			f2 = DistanceAtAngle(inputPoints, templatePoints, x2);
		}
	}
	return std::min(f1, f2);
}

float Dollar1Recognizer::DistanceAtAngle(const std::vector<Vector2>& inputPoints, std::vector<Vector2>& templatePoints, const float theta)
{
	std::vector<Vector2> rotatedInputPoints = RotatePoints(inputPoints, theta);
	const float distance = PathDistance(rotatedInputPoints, templatePoints);
	return distance;
}

float Dollar1Recognizer::PathDistance(std::vector<Vector2>& A, std::vector<Vector2>& B)
{
	const int minLength = std::min(A.size(), B.size());
	float totalDistance = 0;
	for (int i = 0; i < minLength; i++)
	{
		totalDistance += A[i].Distance(B[i]);
	}
	return totalDistance / minLength;
}
