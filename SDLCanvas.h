#pragma once
#include <SDL_video.h>
#include <vector>
#include "Vector2.h"

class SDLCanvas
{
public:
    SDLCanvas(Vector2 size);
    ~SDLCanvas();

    void Render() const;

    void SetBrushWidth(int i);
    void SetBrushColor(int i);

    std::vector<Vector2> GetDrawpoints();
    std::vector<Vector2> GetResampledDrawpoints(int n);

    void StartDrawing(Vector2 hitPoint);
    void EndDrawing();
    void UpdateDrawing(Vector2 hitPoint);

private:
    SDL_Window* m_pDisplay;
    SDL_Surface* m_pSurface;
    int* m_pPixels{};
    Vector2 m_CanvasSize;

    std::vector<Vector2> m_Drawpoints;
    Vector2 m_PreviousHitPoint = ZeroVector2;

    int m_BrushWidth = 10;
    int m_PaintColor = 0x00000000;

    void ResetTexture();
    void PaintTexture(Vector2 hitPoint) const;
    void DrawLine(Vector2 start, Vector2 end) const;

    static std::vector<Vector2> Resample(std::vector<Vector2>& points, int targetPointCount);
    static float PathLength(const std::vector<Vector2>& path);
};