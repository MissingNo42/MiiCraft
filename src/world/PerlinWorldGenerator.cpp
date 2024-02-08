//
// Created by guill on 07/02/2024.
//

#include <fstream>
#include "PerlinWorldGenerator.h"


PerlinWorldGenerator::PerlinWorldGenerator() {
    generateNoise();
}



void PerlinWorldGenerator::generateChunk(World& w , const t_pos2D pos) {
    Block blocks[16][128][16];

    //Append in a file

    int blockHeights[16][16];

    for(int i = 0; i < 16; i ++){
        for(int j = 0; j < 16; j++){

            //TODO : A revoir ptdr




            double d = noise.GetNoise((float) (pos.x * 16 + i) / 16.0f, (float) (pos.y * 16 + j) / 16.0f) *100 +25;
            if(d < 0){
                d = 0;
            } else if(d> 127){
                d = 127;
            }



            int height = (int) d;


            //On remplit le tableau des hauteurs
            blockHeights[i][j] = height;



            for(int k = 0; k < 128; k++){
                if (k == 0){
                    blocks[i][k][j].type = BlockType::Bedrock;
                }
                else if(k < height /2){
                    blocks[i][k][j].type = BlockType::Stone;
                }else if(k < height){
                    blocks[i][k][j].type = BlockType::Dirt;
                }else if(k == height){
                    blocks[i][k][j].type = BlockType::Grass;
                } else {
                    blocks[i][k][j].type = BlockType::Air;
                }

            }

        }
    }

    //On sélectionne une coordonnée aléatoire du tableau des hauteurs
    int x = rand() %12 + 2;
    int z = rand() %12 +2;

    //On construit le tronc :
    for (int y = 0; y < 5; y++) {
        blocks[x][blockHeights[x][z] + y][z].type = BlockType::Log;
    }

    //On construit les feuilles :
    for(int h = 0; h < 2; h++){
        for (int i = -2; i < 3; i++) {
            for (int j = -2; j < 3; j++) {
                if(i == 0 && j == 0){
                    continue;
                }
                blocks[x + i][blockHeights[x][z] + 2 + h][z + j].type = BlockType::Leaves;
            }
        }
    }
    for(int i = -1; i < 2; i++){
        for(int j = -1; j < 2; j++){
            if(i == 0 && j == 0){
                continue;
            }
            blocks[x + i][blockHeights[x][z] + 4][z + j].type = BlockType::Leaves;
        }
    }


    blocks[x + 1][blockHeights[x][z] + 5][z].type = BlockType::Leaves;
    blocks[x - 1][blockHeights[x][z] + 5][z].type = BlockType::Leaves;
    blocks[x][blockHeights[x][z] + 5][z+1].type = BlockType::Leaves;
    blocks[x][blockHeights[x][z] + 5][z-1].type = BlockType::Leaves;







    VerticalChunk* vc = new VerticalChunk(blocks);
    w.addChunk(pos, vc);

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
    noise.SetSeed(1345); //TODO : A changer
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
