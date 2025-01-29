#pragma once

#include "Chunk.h"
#include "WorldGenerator.h"
#include "utils/FastNoiseLite.h"
#include "world/biome/Tergen.h"


class PerlinWorldGenerator final : public WorldGenerator {
private:
//    FastNoiseLite noise;

    FastNoiseLite noiseErosion;
    FastNoiseLite noiseTemperature;
    FastNoiseLite noiseHumidity;
    FastNoiseLite noiseAltitude;
    FastNoiseLite noiseContinental;

	void buildTree(BlockCoord pos, Chunk& vc);
public:
    PerlinWorldGenerator();
    void initNoise();

    BiomeType guessBiome(float ero, float temp, float hum, float cont, int height);
    void generateChunk(Chunk& vc) override;
};
