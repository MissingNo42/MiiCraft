//
// Created by Martin on 08/02/2024.
//

#ifndef MIICRAFTTEST_SAVEMANAGER_H
#define MIICRAFTTEST_SAVEMANAGER_H

#include "../world/verticalChunk.h"
#include <gccore.h>
#include <fat.h>

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class saveManager {
    static saveManager* instance;
public:
    saveManager();
    static saveManager* getInstance();

    /// Sauvegarde un chunk dans un fichier nommé selon sa position, crée le fichier si il n'existe pas
    /// \param chunkPos position du chunk
    /// \param c chunk a sauvegarder
    void saveChunk(t_pos2D pos, VerticalChunk* c);
    /// retourne si le fichier correspondant au chunk existe
    bool isChunkSaved(t_pos2D chunkPos);
    /// Charge un chunk sans vérifier si il existe
    /// \param chunkPos
    /// \return null si problème de lecture
    VerticalChunk* loadChunk(t_pos2D chunkPos);
};


#endif //MIICRAFTTEST_SAVEMANAGER_H
