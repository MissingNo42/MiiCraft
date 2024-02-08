//
// Created by Martin on 08/02/2024.
//

#include "saveManager.h"

saveManager* saveManager::getInstance() {
    if (instance == nullptr)
    {
        instance = new saveManager();
    }
    return instance;
}

void saveManager::saveChunk(t_pos2D pos, VerticalChunk *c) {
    char filename[30];
    sprintf(filename, "%d_%d.chunk", pos.x, pos.y);
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {printf("ouverture du fichier %d %d foiree \r", pos.x, pos.y); return;}

    printf("=====1======\r");

    const char* str = c->toString();

    fwrite(str, 16*16*128, 1, file);


//    for (int i = 0; i < 16*16*128; ++i) {
//        fprintf(file, "%u", str[i]);
//    }


//    printf("chunk sauvegarde sous %s\r", filename);
    fclose(file);
}

bool saveManager::isChunkSaved(t_pos2D chunkPos) {
    return false;
}

void saveManager::loadChunk(t_pos2D pos, VerticalChunk* c) {
    char filename[30];
    sprintf(filename, "%d_%d.chunk", pos.x, pos.y);
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {printf("ouverture du fichier %d %d foiree \r", pos.x, pos.y); return;}

    fread(c->blocks, 16*16*128, 1, file);
}

saveManager::saveManager() {
    if (!fatInitDefault()) {
        printf("fatInitDefault failure: terminating\r");
        return;
    }
}
