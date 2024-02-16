//
// Created by guill on 07/02/2024.
//

#include <queue>
#include "PerlinWorldGenerator.h"


PerlinWorldGenerator::PerlinWorldGenerator() {
    initNoise();
}

float smoothstep(float edge0, float edge1, float x) {
    // Scale, bias and saturate x to 0..1 range
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}

void PerlinWorldGenerator::generateChunk(World& w , const t_pos2D pos) {
    VerticalChunk* vc = new VerticalChunk();

    //Append in a file

    u8        heightMap[16][16];
    BiomeType biomeMap [16][16];

    for(int i = 0; i < 16; i ++){
        for(int j = 0; j < 16; j++){

            float block_x = (float) (pos.x * 16 + i);
            float block_y = (float) (pos.y * 16 + j);


            BiomeType biome;
            int height; // Hauteur en bloc de la colonne de blocs
            //Propriétés du terrain
            float altitude = noiseAltitude.GetNoise(block_x, block_y) + 1.;
            float erosion = noiseErosion.GetNoise(block_x, block_y) + 1.;
            float temperature = noiseTemperature.GetNoise(block_x, block_y) + 1.;
            float humidity = noiseHumidity.GetNoise(block_x, block_y) + 1.;
            float continent = noiseContinental.GetNoise(block_x, block_y) + 1.;
            altitude /= 2; // [0, 1]

            height = (BiomeGenerator::peakAmplitude * erosion) * altitude
                     + smoothstep(.3, 1.2, continent) * (seaLevel + BiomeGenerator::continentLevel)
                     + BiomeGenerator::bottomLevel;
//            height = (erosion * altitude * continent * 15.f + 2.);
            height = std::clamp(height, 0, 127);

//            printf("%d\r", height);
            biome = guessBiome(erosion, temperature, humidity, continent, height);
            BiomeGenerator::generateDesert(vc, i, j, height);


            //On remplit le tableau des hauteurs
            biomeMap [i][j] = biome;
            heightMap[i][j] = height;
        }
    }

    //On sélectionne une coordonnée aléatoire du tableau des hauteurs
    int x = rand() %12 + 2;
    int z = rand() %12 +2;
    int y = heightMap[x][z] + 1;

    //On construit un arbre à cette position
    buildTree({x, y, z}, vc);

    w.initLight(vc);
    w.addChunk(pos, vc);
    w.setNeighboors(pos, vc);
    std::cout << "Chunk generated at " << pos.x << " " << pos.y << " with id : " << vc->id << std::endl;
    std::cout << " with neighboors : " << std::endl;
    for(int i = 0; i < 4; i++){
        if(vc->VC_GetNeighboors() != nullptr){
            std::cout << "Neighboor " << i << " : " << vc->VC_GetNeighboor(i)->id << std::endl;
        }
    }

    //print the whole chunk
//    for(int i = 0; i < 16; i++){
//        for(int j = 0; j < 16; j++){
//            for(int k = 0; k < 128; k++){
//                printf("Block at %d %d %d : %d\r",i,j,k,vc->VC_GetBlock({i, k, j}).type);
//            }
//        }
//    }

}

//void PerlinWorldGenerator::generateNoise() {
//    noise.SetSeed(Random::getSeed());
//    /*
//    noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
//    noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_EuclideanSq);
//    noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Div);
//    noise.SetFrequency(0.05f);
//    noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Add);
//     */
//
//    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
//    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
//    noise.SetFractalOctaves(4);
//    noise.SetFractalLacunarity(5.0);
//    noise.SetFractalGain(0.5);
//    noise.SetFrequency(0.01f);
//
//}

void PerlinWorldGenerator::initNoise() {
//    biomeNoise.SetFrequency(.01);
//    biomeNoise.SetSeed(Random::getSeed());//  Game::getInstance()->getSeed());
//
//    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
//    biomeNoise.SetFractalType(FastNoiseLite::FractalType_None);
//    biomeNoise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Hybrid);
//    biomeNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
//    biomeNoise.SetCellularJitter(1.);

//    //Continental
//    noiseContinental.SetFrequency(.015);
//    noiseContinental.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
//    noiseContinental.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_EuclideanSq);
//    noiseContinental.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
//    noiseContinental.SetCellularJitter(3.333);
//    noiseContinental.SetDomainWarpType(FastNoiseLite::DomainWarpType_BasicGrid);
//    noiseContinental.SetDomainWarpAmp(59);
//    noiseContinental.SetDomainWarpFrequency(.09);

    noiseContinental.SetFrequency(.007);
    noiseContinental.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);

    //Erosion
    noiseErosion.SetFrequency(.022 / 16.f);
    noiseErosion.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S); //Peut-être mettre en non S
    //Temperature
    noiseTemperature.SetFrequency(.02);
    noiseTemperature.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
    //Humidity
    noiseHumidity.SetFrequency(.01);
    noiseHumidity.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    //Altitude (main)
    noiseAltitude.SetFrequency(0.025f / 16.f);
    noiseAltitude.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noiseAltitude.SetFractalType(FastNoiseLite::FractalType_FBm);
    noiseAltitude.SetFractalOctaves(3);
    noiseAltitude.SetFractalLacunarity(5.75);
    noiseAltitude.SetFractalGain(0.5);
}

void PerlinWorldGenerator::buildTree(t_coord pos, VerticalChunk* vc) {
    int height = rand() % 4 + 1;
    //On construit le tronc :
    for (int y = 0; y < 3+height ; y++) {
        vc->VC_SetBlock({pos.x, pos.y + y, pos.z}, BlockType::WoodOak);
    }

    //On construit les feuilles :
    for(int h = 0; h < 2; h++){
        for (int i = -2; i < 3; i++) {
            for (int j = -2; j < 3; j++) {
                if(i == 0 && j == 0){
                    continue;
                }
                if ((i == 2 || i == -2) && ( j == 2 || j == -2)) {
                    if (rand() %2)
                        vc->VC_SetBlock({pos.x + i, pos.y + height + h, pos.z + j}, BlockType::LeaveOak);
                }
                else
                    vc->VC_SetBlock({pos.x + i, pos.y + height + h, pos.z + j}, BlockType::LeaveOak);
            }
        }
    }
    for(int i = -1; i < 2; i++){
        for(int j = -1; j < 2; j++){
            if(i == 0 && j == 0){
                continue;
            }
            if ((i == 1 || i == -1) && ( j == 1 || j == -1)) {
                if (rand() %2)
                    vc->VC_SetBlock({pos.x + i, pos.y + 2 + height, pos.z + j}, BlockType::LeaveOak);
            }
            else
                vc->VC_SetBlock({pos.x + i, pos.y + 2 + height, pos.z + j}, BlockType::LeaveOak);
        }
    }

    vc->VC_SetBlock({pos.x + 1, pos.y + 3 + height, pos.z}, BlockType::LeaveOak);
    vc->VC_SetBlock({pos.x - 1, pos.y + 3 + height, pos.z}, BlockType::LeaveOak);
    vc->VC_SetBlock({pos.x, pos.y + 3 + height, pos.z+1}, BlockType::LeaveOak);
    vc->VC_SetBlock({pos.x, pos.y + 3 + height, pos.z-1}, BlockType::LeaveOak);
    vc->VC_SetBlock({pos.x, pos.y + 3 + height, pos.z}, BlockType::LeaveOak);

}



void PerlinWorldGenerator::propagateLightToNeighbor(VerticalChunk* c, const t_coord& neighbor, int CurrentLightValue, std::queue<t_coord>& lightQueue) {
    if (c->VC_GetBlock(neighbor).type < CurrentLightValue || c->VC_GetBlock(neighbor).type >= 16) {
        c->VC_SetBlock(neighbor, static_cast<BlockType>(CurrentLightValue));
        lightQueue.push(neighbor);
    }
}



BiomeType PerlinWorldGenerator::guessBiome(float ero, float temp, float hum, float cont, int height) {
    if      (cont < 1.2 && height < seaLevel + BiomeGenerator::bottomLevel)     {return BiomeType::Ocean;}
    else if (height < seaLevel + BiomeGenerator::bottomLevel + 4) {return BiomeType::Beach;}
    else
    {// CONTINENT
        return BiomeType::Plain;
    }
}
