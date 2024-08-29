#include "../Canvas.h"

void Paint() {
    double t = ImGui::GetTime();

    SetFillColor(0, 0, 0);
    Text(4, 4, "Hello World, %.1f", t);

    for (int i = 0; i < 20; i++) {
        SetFillColor(sin(i + t * 4.567) * 0.5 + 0.5, sin(i + 10 + t * 10.1234) * 0.5 + 0.5, sin(i + 20 + t * 15.356) * 0.5 + 0.5);

        Line(float2(sin(t + i * 0.13 + 1) * 300, cos(t + i * 0.08 + 2) * 300) + 400.0f,
             float2(cos(t + i * 0.25 + 8) * 300, sin(t + i * 0.18 + 6) * 300) + 350.0f, 2.0f);
    }
}