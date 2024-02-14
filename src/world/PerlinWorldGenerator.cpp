//
// Created by guill on 07/02/2024.
//

#include "world/PerlinWorldGenerator.h"


PerlinWorldGenerator::PerlinWorldGenerator() {
    initNoise();
}



void PerlinWorldGenerator::generateChunk(World& w , const t_pos2D pos) {
    VerticalChunk* vc = new VerticalChunk();

    //Append in a file

    int blockHeights[16][16];

    for(int i = 0; i < 16; i ++){
        for(int j = 0; j < 16; j++){

            float block_x = (float) (pos.x * 16 + i);
            float block_y = (float) (pos.y * 16 + j);

            BiomeType biome;

            float altitude = noiseAltitude.GetNoise(block_x, block_y) + 1.;

            float erosion = noiseErosion.GetNoise(block_x, block_y) + 1.;
            float temperature = 0;//noiseTemperature.GetNoise(block_x, block_y) + 1.;
            float humidity = 0;//noiseHumidity.GetNoise(block_x, block_y) + 1.;
            float continent = 0;//noiseContinental.GetNoise(block_x, block_y) + 1.;

            biome = guessBiome(erosion, temperature, humidity, continent);

//            double d = noise.GetNoise(block_x / 16.0f, block_y / 16.0f) *100 +25;

            int height = (erosion * altitude * 30.f + 2.);
            height = std::clamp(height, 0, 127);

            BiomeGenerator::generateDesert(vc, i, j, height);


//            switch (biome) {
//                case Ocean:
//                    break;
////                case Beach:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
////                case Savanna:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
////                case Tundra:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
////                case Desert:
//////                    generateDesert(vc, block_x, block_y, height);
////                    generateVide__(vc, block_x, block_y, height);
////                    break;
////                case Plain:
//////                    generatePlain(vc, block_x, block_y, height);
////                    generateVide__(vc, block_x, block_y, height);
////                    break;
////                case WoodedPlain:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
////                case Hills:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
////                case WoodedHills:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
////                case Badlands:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
////                case WoodedBadlands:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
////                case Jungle:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
////                case Mushroom:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
////                case IcePeak:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
////                case Taiga:
////                    generateVoid(vc, block_x, block_y, height);
////                    break;
//                default:
//                    t_coord p(i, 1, j);
//                    vc->VC_SetBlock(p, BlockType::Bedrock);
//            }

            //On remplit le tableau des hauteurs
            blockHeights[i][j] = height;
        }
    }

    //On sélectionne une coordonnée aléatoire du tableau des hauteurs
    int x = rand() %12 + 2;
    int z = rand() %12 +2;
    int y = blockHeights[x][z] + 1;

    //On construit un arbre à cette position
    buildTree({x, y, z}, vc);


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
    //On construit le tronc :
    for (int y = 0; y < 5; y++) {
        vc->VC_SetBlock({pos.x, pos.y + y, pos.z}, BlockType::WoodOak);
    }

    //On construit les feuilles :
    for(int h = 0; h < 2; h++){
        for (int i = -2; i < 3; i++) {
            for (int j = -2; j < 3; j++) {
                if(i == 0 && j == 0){
                    continue;
                }
                vc->VC_SetBlock({pos.x + i, pos.y + 2 + h, pos.z + j}, BlockType::LeaveOak);
            }
        }
    }
    for(int i = -1; i < 2; i++){
        for(int j = -1; j < 2; j++){
            if(i == 0 && j == 0){
                continue;
            }
            vc->VC_SetBlock({pos.x + i, pos.y + 4, pos.z + j}, BlockType::LeaveOak);
        }
    }

    vc->VC_SetBlock({pos.x + 1, pos.y + 5, pos.z}, BlockType::LeaveOak);
    vc->VC_SetBlock({pos.x - 1, pos.y + 5, pos.z}, BlockType::LeaveOak);
    vc->VC_SetBlock({pos.x, pos.y + 5, pos.z+1}, BlockType::LeaveOak);
    vc->VC_SetBlock({pos.x, pos.y + 5, pos.z-1}, BlockType::LeaveOak);
    vc->VC_SetBlock({pos.x, pos.y + 5, pos.z}, BlockType::LeaveOak);
}

BiomeType PerlinWorldGenerator::guessBiome(float ero, float temp, float hum, float cont) {
    if      (cont < .6) {return BiomeType::Ocean;}
    else if (cont < .5) {return BiomeType::Ocean;}
    else
    {// CONTINENT
        if      (temp > 1.5) {return BiomeType::Desert;}
        else// if (temp > .5)
        {return BiomeType::Plain;}
    }
}
