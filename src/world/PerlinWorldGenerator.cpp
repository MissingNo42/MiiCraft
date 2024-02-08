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
    std::ofstream file("chunk.txt", std::ios::app);

    for(int i = 0; i < 16; i ++){
        for(int j = 0; j < 16; j++){

            //TODO : A revoir ptdr




            double d = noise.GetNoise((float) (pos.x * 16 + i) / 16.0f, (float) (pos.y * 16 + j) / 16.0f) *100 +25;
            int height;

           // height = noiseToInteger(d);
            height = (int) d;
            file << height << std::endl;
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
                }else{
                    blocks[i][k][j].type = BlockType::Air;
                }

            }

        }
    }
    file.close();

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



}
