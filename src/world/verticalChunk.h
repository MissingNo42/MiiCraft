//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_VERTICALCHUNK_H
#define MIICRAFT_VERTICALCHUNK_H

#include "coord.h"
#include "horizontalChunck.h"

struct t_pos2D{
    short x;
    short y;
    t_pos2D();
    t_pos2D(short x, short y);
    bool const operator==(const t_pos2D& p) const;
    bool const operator<(const t_pos2D& p) const;
};


class VerticalChunk {

private:
//    t_coord coords;
    std::vector<HorizontalChunk*> chunks;
public:

    VerticalChunk();

    void VC_SetBlock(t_coord coord, BlockType block);


};


#endif //MIICRAFT_VERTICALCHUNK_H
