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


float easeInOutCubic(float x) {
    float tmp = -2.f * x + 2.f;
    return x < 0.5f ? 4.f * x * x * x : 1.f - tmp * tmp * tmp / 2.f;
}

float easeInOutQuint(float x) {
    float tmp = -2.f * x + 2.f;
    return x < 0.5f ? 16.f * x * x * x * x * x : 1 - tmp * tmp * tmp * tmp * tmp / 2.f;
}

float erosionRepartition(float x) {
    if (x < .85f) {return 0.3f * x + .05f;}
    else if (x > 1.64f) { float result = x * 3.5f - 4.4f; return result < 2.f ? result : 2.f ;}
    return 1.3f * x -.8f;
}

void PerlinWorldGenerator::generateChunk(World& w , const t_pos2D pos) {
    VerticalChunk* vc = new VerticalChunk();

    w.setNeighboors(pos, vc);
    w.addChunk(pos, vc);
    //Append in a file

    u8        heightMap[16][16];
    u16 biomeRepartition[BIOME_COUNT];           for (int i = 0; i < BIOME_COUNT; ++i) {biomeRepartition[i] = 0;}
    f64       humidityMean = 0;

    for(int i = 0; i < 16; i ++){
        for(int j = 0; j < 16; j++){

            float block_x = (float) (pos.x * 16 + i);
            float block_y = (float) (pos.y * 16 + j);


            BiomeType biome;
            int height; // Hauteur en bloc de la colonne de blocs
            //Propriétés du terrain
            float altitude = noiseAltitude.GetNoise(block_x, block_y) + 1.;
            float erosion = noiseErosion.GetNoise(block_x, block_y) + 1.;
            float shoreLevel = std::clamp((erosion - 1.f) * 1.5f + 1.f, 0.f, 2.f);
            float temperature = std::clamp(noiseTemperature.GetNoise(block_x, block_y) * 1.75f, -1.f, 1.f) + 1.;
            float humidity = noiseHumidity.GetNoise(block_x, block_y) + 1.;
            float continent = noiseContinental.GetNoise(block_x, block_y) + 1.;
            altitude = easeInOutCubic(altitude / 2.f);
            erosion = erosionRepartition(erosion);


            height = (Tergen::peakAmplitude * erosion) * altitude
                     + smoothstep(.3, 1.2, continent) * (Tergen::seaLevel + Tergen::continentLevel * shoreLevel)
                     + Tergen::bottomLevel;
//            height = (erosion * altitude * continent * 15.f + 2.);
            height = std::clamp(height, 0, 127);

//            printf("%d\r", height);
            biome = guessBiome(erosion, temperature, humidity, continent, height);
//            if (biome == BiomeType::Ocean) {Tergen::generateOcean(vc, i, j, height, w.lightQueue);}
//            else if (biome == BiomeType::Beach) {Tergen::generateBeach(vc, i, j, height, w.lightQueue);}
//            else
//            {
//                erosion = temperature;
////                height = 10;
//                if (erosion < 0) {Tergen::generateTundra(vc, i, j, height, w.lightQueue);}
//                else if (erosion < 1.0) {Tergen::generatePlain(vc, i, j, height,w.lightQueue);}
//                else if (erosion < 1.5) {Tergen::generateDesert(vc, i, j, height, w.lightQueue);}
//                else if (erosion <= 2.) //ne pas enlever
//                {Tergen::generateSavanna(vc, i, j, height,w.lightQueue);}
//            }
            switch (biome) {
                case Ocean: Tergen::generateOcean(vc, i, j, height, w.lightQueue);
                    break;
                case Beach: Tergen::generateBeach(vc, i, j, height, w.lightQueue);
                    break;
                case Savanna: Tergen::generateSavanna(vc, i, j, height, w.lightQueue);
                    break;
                case Tundra: Tergen::generateTundra(vc, i, j, height, w.lightQueue);
                    break;
                case Desert: Tergen::generateDesert(vc, i, j, height, w.lightQueue);
                    break;
                case Plain: case Hills: Tergen::generatePlain(vc, i, j, height, w.lightQueue);
                    break;
                case WoodedPlain: case WoodedHills: Tergen::generateForest(vc, i, j, height, w.lightQueue);
                    break;

                case StonyLand: Tergen::generateWindSwept(vc, i, j, height, w.lightQueue);
                    break;
                case Badlands: Tergen::generateBadLand(vc, i, j, height, w.lightQueue);
                    break;
                case WoodedBadlands: Tergen::generateBadLand(vc, i, j, height, w.lightQueue);
                    break;
                case Jungle: Tergen::generateJungle(vc, i, j, height, w.lightQueue);
                    break;
                case DarkForest: Tergen::generateDarkForest(vc, i, j, height, w.lightQueue);
                    break;
                case IcePeak: Tergen::generateIcy(vc, i, j, height, w.lightQueue);
                    break;
                case Taiga: Tergen::generateTaiga(vc, i, j, height, w.lightQueue);
                    break;
                case StonyShore: Tergen::generateStonyShore(vc, i, j, height, w.lightQueue);
                    break;
                case RedBeach:
                    Tergen::generateRedBeach(vc, i, j, height, w.lightQueue);
                    break;
                case Void:
                    Tergen::generateVoid(vc, i, j, height, w.lightQueue);
                    break;
            }


            //On remplit le tableau des hauteurs
            biomeRepartition[biome]++;
            heightMap[i][j] = height;
            humidityMean += humidity;
        }
    }

    if (biomeRepartition[Tundra] >= 128)
    {
        int proba = (int)((float)biomeRepartition[Tundra] / 256.f * 10.f);
        if (rand()%200 < proba)
        {
            t_coord iglooPos{(pos.x * 16 + 8), heightMap[8][8] - 1, (pos.y * 16 + 8)};
            StructBuilder::generateIgloo(w, iglooPos);
        }
    }
    if (biomeRepartition[Savanna] >= 128)
    {
        int proba = (int)((float)biomeRepartition[Savanna] / 256.f * 10.f);
        if (rand()% 25 < proba)
        {
            t_coord acaciaPos{(pos.x * 16 + 7 + rand()%3), heightMap[8][8], (pos.y * 16 + 7 + rand()%3)};
            StructBuilder::generateAcacia(w, acaciaPos);
        }
    }
//    if (biomeRepartition[DarkForest] >= 64)
//    {
//        int nbTry = humidityMean * 2 - 1.8f;
//        for (int i = 0; i < nbTry; ++i) {
//            t_coord mushroomPos{pos.x * 16 + rand()%10 + 3, heightMap[8][8], (pos.y * 16 + rand()%10 + 3)};
//            if (rand() % 2 == 0)
//            {
//                StructBuilder::generateRedMushroom(w, mushroomPos);
//            }
//            else
//            {
//                StructBuilder::generateBrownMushroom(w, mushroomPos);
//            }
//        }
//    }

    humidityMean /= 16*16;
    int treeAttempts = humidityMean * 3;
    for (int i = 0; i < treeAttempts; ++i)
    {

        //On sélectionne une coordonnée aléatoire du tableau des hauteurs
        int x = rand() %10 + 3;
        int z = rand() %10 + 3;
        int y = heightMap[x][z] + 1;


        t_coord treePos((pos.x * 16 + x), y, (pos.y * 16 + z));
        if (StructBuilder::checkClassicTree(w, treePos))
        {
            u16 b;
            int treeChoice = rand() % 256;
            int cumulProba = 0;
            for (b = 0; b < BIOME_COUNT; ++b) {
                cumulProba += biomeRepartition[b];
                if (treeChoice <= cumulProba)
                {
                    break;
                }
            }
            BiomeType biomeTree = (BiomeType)b;
            switch (biomeTree) {
                case Void:
                case Ocean:
                case Savanna:
                case Tundra:
                case Plain:
                case Hills:
                case Beach:
                case IcePeak:
                case StonyShore:
                case StonyLand:
                    break;
                case Desert:
                case Badlands:
                    StructBuilder::generateCactus(w, treePos);
                    break;
                case WoodedPlain:
                case WoodedHills:
                    StructBuilder::generateStdTree(w, treePos);
                    break;
                case WoodedBadlands:
                    StructBuilder::generateStdTree(w, treePos, DryOakTree);
                    break;
                case Jungle:
                    StructBuilder::generateStdTree(w, treePos, SakuraTree);
                    break;
                case DarkForest:
                    StructBuilder::generateStdTree(w, treePos, BirchTree);
                    break;
                case Taiga:
                    StructBuilder::generateSpruce(w, treePos);
                    break;
            }

        }

    }

    //    w.initLight(vc);
    w.propagateLight(vc,w.lightQueue);
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
    noiseErosion.SetFrequency(.004); // On garde
    noiseErosion.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S); //Peut-être mettre en non S
    //Temperature
    noiseTemperature.SetFrequency(.012);
    noiseTemperature.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
    //Humidity
    noiseHumidity.SetFrequency(.01);
    noiseHumidity.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    //Altitude (main)
    noiseAltitude.SetFrequency(0.025f / 6.f);
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




BiomeType PerlinWorldGenerator::guessBiome(float ero, float temp, float hum, float cont, int height) {
    // cont < 1.0f || <- falaises
    if      (height < Tergen::seaLevel - 1 + Tergen::bottomLevel)
    { // ===== OCEAN =====
        return BiomeType::Ocean;
    }
    else if (cont < 1.14f && height < Tergen::seaLevel + Tergen::bottomLevel + 4)
    {// ===== PLAGE =====
        if (ero > 1.4 && temp < 1.3)
        {return BiomeType::StonyShore;}
        else if (temp > 1.6f)
        {return BiomeType::RedBeach;}
        else
        {return BiomeType::Beach;}
    }
    else
    {// ===== CONTINENT =====
        // Cold biomes
        if (temp < .5)
        {
            if (ero < 1.8)
            {
                if (hum > 1.) {return Taiga;}
                else          {return Tundra;}
            }
            else {return IcePeak;}
        }
        //Flat hot biomes
        else if (temp > 1.3 && ero < .8)
        {
            if (hum < .7)        {return Desert;}
            else if (hum < 1.4 ) {return Savanna;}
            else                 {return Jungle;}
        }
        //hilly hot biomes
        else if (temp > 1.55 && ero >= .8 )
        {
            if (hum < .7)       {return Badlands;}
            else if (hum < 1.4) {return WoodedBadlands;}
            return BiomeType::WoodedBadlands;
        }
        //Temperate biomes
        else
        {
            if (hum < .4) {return StonyLand;}
            else if (hum < 1.)
            {
                if (ero < 1.) {return Plain;}
                else          {return Hills;}
            }
            else
            {
                if (ero < .75 && hum > 1.45) {return DarkForest;}
                else if (ero < 1.)           {return WoodedPlain;}
                else                         {return WoodedHills;}
            }


        }
    }
    return BiomeType::Void;
}
