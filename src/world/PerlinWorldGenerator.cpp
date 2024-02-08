//
// Created by guill on 07/02/2024.
//

#include "PerlinWorldGenerator.h"


PerlinWorldGenerator::PerlinWorldGenerator() {
    generateNoise();
}



void PerlinWorldGenerator::generateChunk(World& w , const t_pos2D pos) {
    Block blocks[16][128][16];

    for(int i = 0; i < 16; i ++){
        for(int j = 0; j < 16; j++){

            //TODO : A revoir ptdr



            double d = noise.GetNoise((float) (pos.x * 16 + i) / 16.0f, (float) (pos.y * 16 + j) / 16.0f) *100 +25;
            if(d < 0){
                d = 0;
            } else if(d>128) {
                d = 128;
            }
            int height = (int) (d );
            std::cout << height << std::endl;
            for(int k = 0; k < 128; k++){
                if(k < height){
                    blocks[i][k][j].type = BlockType::Stone;
                }else if(k == height){
                    blocks[i][k][j].type = BlockType::Grass;
                }else{
                    blocks[i][k][j].type = BlockType::Air;

                }

            }

        }
    }

    VerticalChunk* vc = new VerticalChunk(blocks);
    w.addChunk(pos, vc);

}

void PerlinWorldGenerator::generateNoise() {
    noise.SetSeed(34567678); //TODO : A changer
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4);
    noise.SetFractalLacunarity(5.0);
    noise.SetFractalGain(0.5);
    //noise.SetFractalWeightedStrength(5);
}
