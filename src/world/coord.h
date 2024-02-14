//
// Created by guill on 06/02/2024.
//

#ifndef WII_COORD_H
#define WII_COORD_H

struct t_coord{
    t_coord(int x, int y, int z);
    [[nodiscard]] bool equals(t_coord coord) const {
        return (x == coord.x && y == coord.y && z == coord.z);
    }
    int x;
    int y;
    int z;
};




#endif //WII_COORD_H
