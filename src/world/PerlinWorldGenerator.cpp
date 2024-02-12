//
// Created by guill on 07/02/2024.
//

#include <fstream>
#include "PerlinWorldGenerator.h"
#include "../system/Random.h"


PerlinWorldGenerator::PerlinWorldGenerator() {
    generateNoise();
}



void PerlinWorldGenerator::generateChunk(World& w , const t_pos2D pos) {
    Block blocks[16][128][16];
    VerticalChunk* vc = new VerticalChunk();

    //Append in a file

    int blockHeights[16][16];

    for(int i = 0; i < 16; i ++){
        for(int j = 0; j < 16; j++){

            //TODO : A revoir ptdr

            float block_x = (float) (pos.x * 16 + i);
            float block_y = (float) (pos.y * 16 + j);

            double d = noise.GetNoise(block_x / 16.0f, block_y / 16.0f) *100 +25;
            if(d < 0){
                d = 0;
            } else if(d> 127){
                d = 127;
            }
            int height = (int) d;

            BiomeType biome = biomeGen.GetBiomeAtBlock(block_x , block_y);


            //On remplit le tableau des hauteurs
            blockHeights[i][j] = height;

            for(int k = 0; k < 128; k++){
                if (k <2){
                    blocks[i][k][j].type = BlockType::Bedrock;
                }
                else if (k < 5){
                    blocks[i][k][j].type = rand() % 2 == 0 ? BlockType::Stone : BlockType::Bedrock;
                }
                else if(k < height /1.2){
                    blocks[i][k][j].type = BlockType::Stone;
                }else if(k < height){

                    blocks[i][k][j].type = BlockType::Dirt;

                }else if(k == height){
                    blocks[i][k][j].type = BlockType::Grass;
                } else {
                    blocks[i][k][j].type = BlockType::Air;
                }
                vc->VC_SetBlock({i, k, j}, blocks[i][k][j].type);

            }

        }
    }
    int o;
    //On sélectionne une coordonnée aléatoire du tableau des hauteurs
    for (o=0; o< rand() % 5;o++) {
        int x = rand() % 12 + 2;
        int z = rand() % 12 + 2;
        int y = blockHeights[x][z] + 1;
        buildTree({x, y, z}, vc);
    }

    //On construit un arbre à cette position



    w.addChunk(pos, vc);
    w.setNeighboors(pos, vc);
    std::cout << "Chunk generated at " << pos.x << " " << pos.y << " with id : " << vc->id << std::endl;
    std::cout << " with neighboors : " << std::endl;
    for(int i = 0; i < 4; i++){
        if(vc->VC_GetNeighboors() != nullptr){
            std::cout << "Neighboor " << i << " : " << vc->VC_GetNeighboor(i)->id << std::endl;
        }
    }


}

int PerlinWorldGenerator::noiseToInteger(float floatValue) {
    if (floatValue < -1.0f)
        floatValue = -1.0f;
    else if (floatValue > 1.0f)
        floatValue = 1.0f;

    int intValue = static_cast<int>((floatValue + 1.0f) );

    return intValue;
}

void PerlinWorldGenerator::generateNoise() {
    noise.SetSeed(Random::getSeed());
    /*
    noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_EuclideanSq);
    noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Div);
    noise.SetFrequency(0.05f);
    noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Add);
     */

    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4);
    noise.SetFractalLacunarity(5.0);
    noise.SetFractalGain(0.5);
    noise.SetFrequency(0.01f);

}

void PerlinWorldGenerator::buildTree(t_coord pos, VerticalChunk* vc) {
    //On construit le tronc :
    for (int y = 0; y < 5; y++) {
        vc->VC_SetBlock({pos.x, pos.y + y, pos.z}, BlockType::Log);
    }

    //On construit les feuilles :
    for(int h = 0; h < 2; h++){
        for (int i = -2; i < 3; i++) {
            for (int j = -2; j < 3; j++) {
                if(i == 0 && j == 0){
                    continue;
                }
                vc->VC_SetBlock({pos.x + i, pos.y + 2 + h, pos.z + j}, BlockType::Leaves);
            }
        }
    }
    for(int i = -1; i < 2; i++){
        for(int j = -1; j < 2; j++){
            if(i == 0 && j == 0){
                continue;
            }
            vc->VC_SetBlock({pos.x + i, pos.y + 4, pos.z + j}, BlockType::Leaves);
        }
    }

    vc->VC_SetBlock({pos.x + 1, pos.y + 5, pos.z}, BlockType::Leaves);
    vc->VC_SetBlock({pos.x - 1, pos.y + 5, pos.z}, BlockType::Leaves);
    vc->VC_SetBlock({pos.x, pos.y + 5, pos.z+1}, BlockType::Leaves);
    vc->VC_SetBlock({pos.x, pos.y + 5, pos.z-1}, BlockType::Leaves);
    vc->VC_SetBlock({pos.x, pos.y + 5, pos.z}, BlockType::Leaves);
}
