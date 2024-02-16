//
// Created by Romain on 14/02/2024.
//

#ifndef MIICRAFTTEST_CACHE_H
#define MIICRAFTTEST_CACHE_H

#include <gctypes.h>
#include <gcutil.h>

#define OCCLUSION_DARK 0x989c9dff
#define OCCLUSION_MID 0xc9cdceff
#define OCCLUSION_LIGHT 0xffffffff


// TODO test perf between u8 and f32
extern const s8 Normals[] ATTRIBUTE_ALIGN(32);

extern const u32 Lights[][4] ATTRIBUTE_ALIGN(32);

class ChunkCache {

};

#endif //MIICRAFTTEST_CACHE_H
