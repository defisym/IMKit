#pragma once

#include "Utilities/HttpDownloader.h"
#include "Utilities/MapDownloader.h"

struct ViewRect {
    double xMin = 0.0f;
    double xMax = 1.0f;
    double yMin = 0.0f;
    double yMax = 1.0f;
};

template<>
struct std::hash<ViewRect> {
    std::size_t operator()(ViewRect const& s) const noexcept;
};

struct ViewParams {
    bool bDebug = false;

    bool bFitOnScreen = true;
    ViewRect newRect = {};
    ViewRect viewRect = {};
};

template<>
struct std::hash<ViewParams> {
    std::size_t operator()(ViewParams const& s) const noexcept;
};

struct MapParams {
    ViewParams viewParams = {};
    MapDownloadParams mapDownloadParams = { 
        .downloaderParams = {
            .bProxy = false,
            .proxyParams = {},
            .httpParams = {
                .bHttps = true,
                .site = "a.tile.opentopomap.org",
                .getFormat = "/{}/{}/{}.png",
                .userAgent = "HttpDownloader/1.0 (OOSAKA)"}},
        .mapSourceParams = {},
        .mapSaveParams = {} };
};

struct Point {
    double x = 0.0; // longitude, normalized to [ 0 ~ 1 ]
    double y = 0.0; // latitude, normalized to [ 0 ~ 1 ]
    int distance = 0; // distance to start, in meter

    bool bValid() const;
};

template<>
struct std::hash<Point> {
    std::size_t operator()(Point const& s) const noexcept;
};

struct FiberPointList {
    using PointList = std::vector<Point>;
    PointList vec = {};

    bool Read(FILE* fp);
    size_t Write(FILE* fp);

    ViewRect GetRect() const;
    Point GetEventPoint(int length);

    static double GetLongitude(double v);
    static double GetLatiitude(double v);
    static double GetDisance(const double lx, const double ly,
        const double rx, const double ry);
    static double GetDisance(const Point& l, const Point& r);

    void UpdateDistance();

    using UndoList = std::vector<PointList>;
    UndoList undo = {};
    UndoList redo = {};
    void Record();
    void UnDo();
    void ReDo();
};

template<>
struct std::hash<FiberPointList::PointList> {
    std::size_t operator()(FiberPointList::PointList const& s) const noexcept;
};

template<>
struct std::hash<FiberPointList::UndoList> {
    std::size_t operator()(FiberPointList::UndoList const& s) const noexcept;
};

struct D3DContext;
struct MapHandler {
    MapParams mapParams = {};
    std::unique_ptr<TileManager> pTileManager = nullptr;

    bool bPaintMode = false;
    FiberPointList pointList = {};

    void UpdateTileManager(const D3DContext* pCtx);
};
