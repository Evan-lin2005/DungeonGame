#pragma once
#pragma once
#include <vector>
#include <utility>

enum Tile { WALL, FLOOR, STAIR_DOWN };

class MapGenerator {
public:
    MapGenerator(int width, int height, int minRoomSize = 5, int maxRoomSize = 12);
    const std::vector<std::vector<Tile>>& getMap() const;
    std::pair<int, int> getStairs() const;
private:
    int width, height;
    int minRoom, maxRoom;
    std::vector<std::vector<Tile>> map;

    struct Rect { int x, y, w, h; };
    std::vector<Rect> rooms;
    std::pair<int, int> stairsPos;

    void initMap();
    void splitRegion(int x, int y, int w, int h);
    void carveRooms();
    void connectRooms();
    void placeStairs();
    void carveRect(const Rect& r);
    void carveCorridor(std::pair<int, int> a, std::pair<int, int> b);
};
