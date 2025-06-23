#include "MapGenerator.h"
#include <cstdlib>
#include <algorithm>

MapGenerator::MapGenerator(int w, int h, int minR, int maxR)
    : width(w), height(h), minRoom(minR), maxRoom(maxR) {
    initMap();
    splitRegion(1, 1, width - 2, height - 2);
    carveRooms();
    connectRooms();
    placeStairs();
}

const std::vector<std::vector<Tile>>& MapGenerator::getMap() const {
    return map;
}

std::pair<int, int> MapGenerator::getStairs() const {
    return stairsPos;
}

void MapGenerator::initMap() {
    map.assign(height, std::vector<Tile>(width, WALL));
}

void MapGenerator::splitRegion(int x, int y, int w, int h) {
    if (w < minRoom * 2 && h < minRoom * 2) {
        rooms.push_back({ x, y, w, h });
        return;
    }
    bool splitH = (w < h);
    if (splitH) {
        int split = minRoom + rand() % (h - minRoom * 2 + 1);
        splitRegion(x, y, w, split);
        splitRegion(x, y + split, w, h - split);
    }
    else {
        int split = minRoom + rand() % (w - minRoom * 2 + 1);
        splitRegion(x, y, split, h);
        splitRegion(x + split, y, w - split, h);
    }
}

void MapGenerator::carveRooms() {
    for (auto& r : rooms) {
        int rw = std::min(r.w, minRoom);
        int rh = std::min(r.h, minRoom);
        int rx = r.x + rand() % (r.w - rw + 1);
        int ry = r.y + rand() % (r.h - rh + 1);
        carveRect({ rx, ry, rw, rh });
        r = { rx, ry, rw, rh };
    }
}

void MapGenerator::carveRect(const Rect& r) {
    for (int i = r.y; i < r.y + r.h; ++i)
        for (int j = r.x; j < r.x + r.w; ++j)
            map[i][j] = FLOOR;
}

void MapGenerator::connectRooms() {
    for (size_t i = 1;i < rooms.size();++i) {
        auto& a = rooms[i - 1];
        auto& b = rooms[i];
        std::pair<int, int> centerA = { a.x + a.w / 2, a.y + a.h / 2 };
        std::pair<int, int> centerB = { b.x + b.w / 2, b.y + b.h / 2 };
        carveCorridor(centerA, centerB);
    }
}

void MapGenerator::carveCorridor(std::pair<int, int> a, std::pair<int, int> b) {
    int x = a.first, y = a.second;
    while (x != b.first) {
        map[y][x] = FLOOR;
        x += (b.first > x) ? 1 : -1;
    }
    while (y != b.second) {
        map[y][x] = FLOOR;
        y += (b.second > y) ? 1 : -1;
    }
}

void MapGenerator::placeStairs() {
    if (rooms.empty()) return;
    auto& r = rooms[rand() % rooms.size()];
    int sx = r.x + r.w / 2;
    int sy = r.y + r.h / 2;
    stairsPos = { sx, sy };
    map[sy][sx] = STAIR_DOWN;
}


