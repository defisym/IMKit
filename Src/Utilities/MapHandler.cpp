#include "MapHandler.h"

#include <numbers>

#include "GUIContext/Param/Param.h"
#include "IMGuiInterface/D3DContext.h"

std::size_t std::hash<ViewRect>::operator()(ViewRect const& s) const noexcept {
    return GetParamHash(s);
}

std::size_t std::hash<ViewParams>::operator()(ViewParams const& s) const noexcept {
    return GetParamHash(s);
}

bool Point::bValid() const { return distance >= 0; }

std::size_t std::hash<Point>::operator()(Point const& s) const noexcept
{
    return GetParamHash(s);
}

bool FiberPointList::Read(FILE* fp) {
    size_t sz = 0;
    size_t read = fread(&sz, sizeof(sz), 1, fp);
    if (read == 0) { return false; }
    if (sz == 0) { return true; }

    vec.resize(sz);
    read = fread(vec.data(), sizeof(Point) * sz, 1, fp);
    if (read == 0) { return false; }

    return true;
}

size_t FiberPointList::Write(FILE* fp) {
    const size_t sz = vec.size();
    size_t write = fwrite(&sz, sizeof(sz), 1, fp);
    if (write == 0) { return false; }
    if (sz == 0) { return true; }

    write = fwrite(vec.data(), sizeof(Point) * sz, 1, fp);
    if (write == 0) { return false; }

    return true;
}

ViewRect FiberPointList::GetRect() const {
    ViewRect rect = {};
    constexpr static auto ONEPOINT_OFFSET = 0.0001;
    constexpr static auto RECT_OFFSET = 0.00001;

    if (vec.empty()) { return rect; }
    if (vec.size() == 1) {
        const auto& point = vec.front();
        rect.xMin = point.x - ONEPOINT_OFFSET; rect.xMax = point.x + ONEPOINT_OFFSET;
        rect.yMin = point.y - ONEPOINT_OFFSET; rect.yMax = point.y + ONEPOINT_OFFSET;

        return rect;
    }
    const auto& point = vec.front();
    rect.xMin = point.x; rect.xMax = point.x;
    rect.yMin = point.y; rect.yMax = point.y;

    for (auto& it : vec) {
        rect.xMin = std::min(rect.xMin, it.x);
        rect.xMax = std::max(rect.xMax, it.x);
        rect.yMin = std::min(rect.yMin, it.y);
        rect.yMax = std::max(rect.yMax, it.y);
    }

    rect.xMin -= RECT_OFFSET; rect.xMax += RECT_OFFSET;
    rect.yMin -= RECT_OFFSET; rect.yMax += RECT_OFFSET;

    return rect;
}

Point FiberPointList::GetEventPoint(int distance) {
    size_t prev = 0;
    size_t next = 0;

    for (size_t index = 0; index < vec.size(); index++) {
        const auto& point = vec[index];
        if (distance >= point.distance) { prev = index; }
        else { next = index; break; }
    }

    // invalid
    if (next == 0) { return { .x = 0,.y = 0,.distance = -1 }; }

    const auto& prevPoint = vec[prev];
    const auto& nextPoint = vec[next];
    const auto percent = (double(distance - prevPoint.distance))
        / (nextPoint.distance - prevPoint.distance);

    return { .x = prevPoint.x + percent * (nextPoint.x - prevPoint.x),
        .y = prevPoint.y + percent * (nextPoint.y - prevPoint.y),
        .distance = distance };
}

double FiberPointList::GetLongitude(double v) {
    // -180 ~ 180
    constexpr static auto MAX_LONGITUDE = 360 / 2;
    return MAX_LONGITUDE * (v - 0.5);
}

double FiberPointList::GetLatiitude(double v) {
    // -90 ~ 90
    constexpr static auto MAX_LATITUDE = 180 / 2;
    return MAX_LATITUDE * (v - 0.5);
}

// Haversine formula
// https://mopheiok.github.io/spark/distance_latitude_longitude/
double FiberPointList::GetDisance(const double lx, const double ly,
    const double rx, const double ry) {
    auto deg2rad = [](double deg) { return deg * std::numbers::pi / 180.0; };

    const auto lLong = deg2rad(GetLongitude(lx));
    const auto lLat = deg2rad(GetLatiitude(ly));
    const auto rLong = deg2rad(GetLongitude(rx));
    const auto rLat = deg2rad(GetLatiitude(ry));

    const auto longDiff = lLong - rLong;
    const auto latDiff = lLat - rLat;
    const auto rEarth = 6378137; // in meter

    // sin^2( latDiff/2 )
    const auto a = std::pow(std::sin(latDiff / 2), 2);
    // cos(lLat) * cos(rLat) * sin^2( longDiff/2 )
    const auto b = std::cos(lLat) * std::cos(rLat) * std::pow(std::sin(longDiff / 2), 2);
    // 2 * R * arcsin¡Ì(a + b)
    const auto disance = 2 * rEarth * std::asin(std::sqrt(a + b));

    return disance;
}

double FiberPointList::GetDisance(const Point& l, const Point& r) {
    return GetDisance(l.x, l.y, r.x, r.y);
}

void FiberPointList::UpdateDistance() {
    if (vec.size() < 2) { return; }

    for (size_t index = 1; index < vec.size(); index++) {
        const auto& prev = vec[index - 1];
        auto& cur = vec[index];

        const auto distance = GetDisance(prev, cur);
        cur.distance = prev.distance + (int)distance;
    }
}

void FiberPointList::Record() {
    undo.emplace_back(vec);
    redo.clear();
}

void FiberPointList::UnDo() {
    if (undo.empty()) { return; }
    redo.emplace_back(vec);
    vec = undo.back(); undo.pop_back();
}

void FiberPointList::ReDo() {
    if (redo.empty()) { return; }
    undo.emplace_back(vec);
    vec = redo.back(); redo.pop_back();
}

// check hash of default FiberPoint
std::size_t std::hash<FiberPointList::PointList>::operator()(FiberPointList::PointList const& s) const noexcept {
    return GetParamHash(Point{});
}

// does nothing
std::size_t std::hash<FiberPointList::UndoList>::operator()(FiberPointList::UndoList const& s) const noexcept {
    return 0;
}

void MapHandler::UpdateTileManager(const D3DContext* pCtx) {
    pTileManager = std::make_unique<TileManager>(pCtx, mapParams.mapDownloadParams);
}
