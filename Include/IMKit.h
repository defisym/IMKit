#pragma once

// All in one include
#ifndef IMKIT_LIB
#define IMKIT_LIB
#endif

// IMGui
#include "IMGui/imgui.h"
#include "IMPlot/implot.h"

// Macro
#include "macro.h"

// CPUAffinity
#include "CPUAffinity/CPUAffinity.h"

// DataInterface
#include "DataInterface/InfDefinition.h"
#include "DataInterface/DataInterface.h"
#include "DataInterface/FileInterface.h"
#include "DataInterface/ParseInterface.h"

// GUIContext
#include "GUIContent/Components/Basic/ComponentPerformanceBase.h"
#include "GUIContent/Components/Basic/ComponentWindow.h"
#include "GUIContent/Interface/InterfaceLogger.h"
#include "GUIContent/Interface/InterfaceMap.h"

// IMGuiEx
#include "IMGuiEx/AddSpin.h"
#include "IMGuiEx/AddToolTipHint.h"
#include "IMGuiEx/ComboEx.h"
#include "IMGuiEx/DisableHelper.h"
#include "IMGuiEx/EmbraceHelper.h"
#include "IMGuiEx/FontEx.h"
#include "IMGuiEx/I18NInterface.h"
#include "IMGuiEx/IndentHelper.h"
#include "IMGuiEx/LabelMaker.h"
#include "IMGuiEx/LoadTexture.h"
#include "IMGuiEx/SliderSpin.h"

// IMGuiInterface
#include "IMGuiInterface/IMGuiInterface.h"
#include "IMGuiInterface/IMGuiContext.h"
#include "IMGuiInterface/IMGuiDemo.h"
#include "IMGuiInterface/D3DContext.h"

// Internationalization
#include "Internationalization/Internationalization.h"

// Utilities
#include "Utilities/AudioPlayer.h"
#include "Utilities/BackBuffer.h"
#include "Utilities/ConfigHandler.h"
#include "Utilities/DataConverter.h"
#include "Utilities/FilterHandler.h"
#include "Utilities/HttpDownloader.h"
#include "Utilities/Logger.h"
#include "Utilities/MapDownloader.h"
#include "Utilities/MapHandler.h"
#include "Utilities/MeasureHelper.h"
#include "Utilities/Param.h"
#include "Utilities/StringResult.h"
#include "Utilities/TimeStampHelper.h"
#include "Utilities/WaitTimer.h"

// Worker
#include "Worker/WorkerBase.h"
#include "Worker/Thread/ThreadBase.h"
#include "Worker/Thread/ThreadWrapper.h"
#include "Worker/Thread/ThreadHandlerBase.h"
