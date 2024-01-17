#include "SDLCanvas.h"
#include <SDL.h>

SDLCanvas::SDLCanvas(Vector2 size) :
m_CanvasSize(size)
{
    SDL_Init(SDL_INIT_VIDEO);
    const std::string windowTitle = "Testing";
    m_pDisplay = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_CanvasSize.x, m_CanvasSize.y, SDL_WINDOW_SHOWN);
    m_pSurface = SDL_GetWindowSurface(m_pDisplay);
    m_pPixels = static_cast<int*>(m_pSurface->pixels);

    ResetTexture();
}

SDLCanvas::~SDLCanvas()
{
    if (m_pSurface != nullptr) {
        SDL_FreeSurface(m_pSurface);
        m_pSurface = nullptr;
    }

    if (m_pDisplay != nullptr) {
        SDL_DestroyWindow(m_pDisplay);
        m_pDisplay = nullptr;
    }

    if (m_pPixels != nullptr) {
        delete[] m_pPixels;
        m_pPixels = nullptr;
    }
    SDL_Quit();
}
void SDLCanvas::Render() const
{
    SDL_UpdateWindowSurface(m_pDisplay);
}
void SDLCanvas::SetBrushWidth(const int i)
{
    m_BrushWidth = i;
}

void SDLCanvas::SetBrushColor(const int i)
{
    m_PaintColor = i;
}

std::vector<Vector2> SDLCanvas::GetDrawpoints()
{
    return m_Drawpoints;
}

std::vector<Vector2> SDLCanvas::GetResampledDrawpoints(int n)
{
    return Resample(m_Drawpoints, n);
}

void SDLCanvas::StartDrawing(Vector2 hitPoint)
{
    PaintTexture(hitPoint);
}

void SDLCanvas::EndDrawing()
{
    m_Drawpoints.clear();
    ResetTexture();
    m_PreviousHitPoint = ZeroVector2;
}

void SDLCanvas::UpdateDrawing(Vector2 hitPoint)
{
    const bool contains = std::ranges::find(m_Drawpoints, hitPoint) != m_Drawpoints.end();
    if (hitPoint != ZeroVector2 && !contains && 
        hitPoint.x >= 0 && hitPoint.x < m_CanvasSize.x && hitPoint.y >= 0 && hitPoint.y < m_CanvasSize.y)
    {
        if (m_PreviousHitPoint != ZeroVector2)
        {
            DrawLine(m_PreviousHitPoint, hitPoint);
        }

        PaintTexture(hitPoint);
        m_Drawpoints.push_back(hitPoint);

        m_PreviousHitPoint = hitPoint;
    }
}

void SDLCanvas::ResetTexture() {
    for (int i = 0; i < m_CanvasSize.y; i++)
    {
        for (int j = 0; j < m_CanvasSize.x; j++)
        {
            *m_pPixels = 0x00ffffff;
            m_pPixels += 1;
        }
    }
    m_pPixels = static_cast<int*>(m_pSurface->pixels);
}

void SDLCanvas::PaintTexture(Vector2 hitPoint) const
{
    const int centerX = static_cast<int>(hitPoint.x);
    const int centerY = static_cast<int>(hitPoint.y);

    const int radius = m_BrushWidth / 2;
    const int radiusSquared = radius * radius;

    for (int x = centerX - radius; x <= centerX + radius; x++)
    {
        for (int y = centerY - radius; y <= centerY + radius; y++)
        {
            if (x >= 0 && x < m_CanvasSize.x && y >= 0 && y < m_CanvasSize.y)
            {
                const int distanceSquared = (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY);
                if (distanceSquared <= radiusSquared)
                {
                    *(m_pPixels + (y) * static_cast<int>(m_CanvasSize.x) + (x)) = m_PaintColor;
                }
            }
        }
    }
}

void SDLCanvas::DrawLine(Vector2 start, Vector2 end) const
{
    Vector2 direction = (end - start);
    direction.Normalize();
    const float distance = start.Distance(end);

    for (int i = 0; i < distance; i += 1)
    {
	    const Vector2 point = start + direction * i;
        PaintTexture(point);
    }
}

std::vector<Vector2> SDLCanvas::Resample(std::vector<Vector2>& points, const int targetPointCount)
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

float SDLCanvas::PathLength(const std::vector<Vector2>& path)
{
    float totalDistance = 0.0f;
    for (size_t i = 1; i < path.size(); ++i) 
    {
        totalDistance += path[i - 1].Distance(path[i]);
    }
    return totalDistance;
}