#include "DisplayPlot.h"

void SetupAxis(const char* xLabel, const char* yLabel) {
	ImPlot::SetupAxis(ImAxis_X1, xLabel, AXIS_FLAGS);
	ImPlot::SetupAxis(ImAxis_Y1, yLabel, AXIS_FLAGS);
}

bool BeginPlotEx(const char* pTitle, const char* xLabel, const char* yLabel) {
	bool bBegin = ImPlot::BeginPlot(pTitle, PLOT_SIZE, PLOT_FLAGS);
	if (bBegin) { SetupAxis(xLabel, yLabel); }

	return bBegin;
}

bool BeginSubPlotEx(const char* pTitle, const int rows, const int cols) {
    return ImPlot::BeginSubplots(pTitle, rows, cols, PLOT_SIZE);
}
