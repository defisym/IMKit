#include "DisplayPlot.h"

constexpr static ImVec2 PLOT_SIZE = { -1.0f, 300.0f };
constexpr static ImPlotFlags PLOT_FLAGS = ImPlotFlags_NoMenus | ImPlotFlags_Crosshairs;
constexpr static ImPlotAxisFlags AXIS_FLAGS = ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_AutoFit;

void SetupAxis(const char* xLabel, const char* yLabel) {
	ImPlot::SetupAxis(ImAxis_X1, xLabel, AXIS_FLAGS);
	ImPlot::SetupAxis(ImAxis_Y1, yLabel, AXIS_FLAGS);
}

bool BeginPlotEx(const char* title_id, const char* xLabel, const char* yLabel) {
	bool bBegin = ImPlot::BeginPlot(title_id, PLOT_SIZE, PLOT_FLAGS);
	if (bBegin) { SetupAxis(xLabel, yLabel); }

	return bBegin;
}
