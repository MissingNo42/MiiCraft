//
// Created by guill on 07/02/2024.
//

#include "world/PerlinWorldGenerator.h"
#include <algorithm>
#include "world/world.h"
#include "world/biome/StructBuilder.h"
#include "world/biome/Tergen.h"

PerlinWorldGenerator::PerlinWorldGenerator() {
	initNoise();
}

f32 smoothstep(f32 edge0, f32 edge1, f32 x) {
	// Scale, bias and saturate x to 0..1 range
	x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	// Evaluate polynomial
	return x * x * (3 - 2 * x);
}


f32 easeInOutCubic(f32 x) {
	f32 tmp = -2.f * x + 2.f;
	return x < 0.5f ? 4.f * x * x * x : 1.f - tmp * tmp * tmp / 2.f;
}

f32 easeInOutQus32(f32 x) {
	f32 tmp = -2.f * x + 2.f;
	return x < 0.5f ? 16.f * x * x * x * x * x : 1 - tmp * tmp * tmp * tmp * tmp / 2.f;
}

f32 erosionRepartition(f32 x) {
	if (x < .85f) { return 0.3f * x + .05f; }
	else if (x > 1.64f) {
		f32 result = x * 3.5f - 4.4f;
		return result < 2.f ? result : 2.f;
	}
	return 1.3f * x - .8f;
}

void PerlinWorldGenerator::generateChunk(Chunk &vc) {
	printf("gen\r");
	u8 heightMap[16][16];
	u16 biomeRepartition[BIOME_COUNT]{};
	f64 humidityMean = 0;

	for (s32 i = 0; i < 16; i++) {
		for (s32 j = 0; j < 16; j++) {

			const f32 block_x = static_cast<f32>((vc.coord.x << 4) + i);
			const f32 block_y = static_cast<f32>((vc.coord.z << 4) + j);

			s32 height; // Hauteur en bloc de la colonne de blocs
			//Propriétés du terrain
			f32 altitude = noiseAltitude.GetNoise(block_x, block_y) + 1.f;
			f32 erosion = noiseErosion.GetNoise(block_x, block_y) + 1.f;
			const f32 shoreLevel = std::clamp((erosion - 1.f) * 1.5f + 1.f, 0.f, 2.f);
			const f32 temperature = std::clamp(noiseTemperature.GetNoise(block_x, block_y) * 1.75f, -1.f, 1.f) + 1.f;
			const f32 humidity = noiseHumidity.GetNoise(block_x, block_y) + 1.f;
			const f32 continent = noiseContinental.GetNoise(block_x, block_y) + 1.f;
			altitude = easeInOutCubic(altitude / 2.f);
			erosion = erosionRepartition(erosion);


			height = static_cast<s32>(std::round((Tergen::peakAmplitude * erosion) * altitude
			         + smoothstep(.3f, 1.2f, continent) * (Tergen::seaLevel + Tergen::continentLevel * shoreLevel)
			         + Tergen::bottomLevel));
//            height = (erosion * altitude * continent * 15.f + 2.);
			height = std::clamp(height, 0, static_cast<s32>(CHUNK_LIMIT));

//            prs32f("%d\r", height);
			const BiomeType biome = guessBiome(erosion, temperature, humidity, continent, height);
//            if (biome == BiomeType::Ocean) {Tergen::generateOcean(vc, i, j, height);}
//            else if (biome == BiomeType::Beach) {Tergen::generateBeach(vc, i, j, height);}
//            else
//            {
//                erosion = temperature;
////                height = 10;
//                if (erosion < 0) {Tergen::generateTundra(vc, i, j, height);}
//                else if (erosion < 1.0) {Tergen::generatePlain(vc, i, j, height,w.lightQueue);}
//                else if (erosion < 1.5) {Tergen::generateDesert(vc, i, j, height);}
//                else if (erosion <= 2.) //ne pas enlever
//                {Tergen::generateSavanna(vc, i, j, height,w.lightQueue);}
//            }

			switch (biome) {
				case Ocean: Tergen::generateOcean(vc, i, j, height);
					break;
				case Beach: Tergen::generateBeach(vc, i, j, height);
					break;
				case Savanna: Tergen::generateSavanna(vc, i, j, height);
					break;
				case Tundra: Tergen::generateTundra(vc, i, j, height);
					break;
				case Desert: Tergen::generateDesert(vc, i, j, height);
					break;
				case Plain:
				case Hills: Tergen::generatePlain(vc, i, j, height);
					break;
				case WoodedPlain:
				case WoodedHills: Tergen::generateForest(vc, i, j, height);
					break;

				case StonyLand: Tergen::generateWindSwept(vc, i, j, height);
					break;
				case Badlands: Tergen::generateBadLand(vc, i, j, height);
					break;
				case WoodedBadlands: Tergen::generateBadLand(vc, i, j, height);
					break;
				case Jungle: Tergen::generateJungle(vc, i, j, height);
					break;
				case DarkForest: Tergen::generateDarkForest(vc, i, j, height);
					break;
				case IcePeak: Tergen::generateIcy(vc, i, j, height);
					break;
				case Taiga: Tergen::generateTaiga(vc, i, j, height);
					break;
				case StonyShore: Tergen::generateStonyShore(vc, i, j, height);
					break;
				case RedBeach:Tergen::generateRedBeach(vc, i, j, height);
					break;
				case Void:Tergen::generateVoid(vc, i, j, height);
					break;
			}


			//On remplit le tableau des hauteurs
			biomeRepartition[biome]++;
			heightMap[i][j] = height;
			humidityMean += humidity;
		}
	}

	if (biomeRepartition[Tundra] >= 128) {
		s32 proba = (s32) ((f32) biomeRepartition[Tundra] / 256.f * 10.f);
		if (rand() % 200 < proba) {
			StructBuilder::generateIgloo(vc, BlockCoord{heightMap[8][8] - 1, 8, 8});
		}
	}
	if (biomeRepartition[Savanna] >= 128) {
		s32 proba = (s32) ((f32) biomeRepartition[Savanna] / 256.f * 10.f);
		if (rand() % 25 < proba) {
			StructBuilder::generateAcacia(vc, BlockCoord{heightMap[8][8], 7 + rand() % 3, 7 + rand() % 3});
		}
	}
//    if (biomeRepartition[DarkForest] >= 64)
//    {
//        s32 nbTry = humidityMean * 2 - 1.8f;
//        for (s32 i = 0; i < nbTry; ++i) {
//            BlockCoord mushroomPos{pos.x * 16 + rand()%10 + 3, heightMap[8][8], (pos.y * 16 + rand()%10 + 3)};
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

	humidityMean /= 16 * 16;
	s32 treeAttempts = humidityMean * 3;
	for (s32 i = 0; i < treeAttempts; ++i) {

		//On sélectionne une coordonnée aléatoire du tableau des hauteurs
		s32 x = rand() % 10 + 3;
		s32 z = rand() % 10 + 3;
		s32 y = heightMap[x][z] + 1;


		BlockCoord treePos(y, (vc.coord.x * 16 + x), (vc.coord.z * 16 + z));
		if (StructBuilder::checkClassicTree(vc, treePos)) {
			u16 b;
			s32 treeChoice = rand() % 256;
			s32 cumulProba = 0;
			for (b = 0; b < BIOME_COUNT; ++b) {
				cumulProba += biomeRepartition[b];
				if (treeChoice <= cumulProba) {
					break;
				}
			}
			BiomeType biomeTree = (BiomeType) b;
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
				case StonyLand:break;
				case Desert:
				case Badlands:StructBuilder::generateCactus(vc, treePos);
					break;
				case WoodedPlain:
				case WoodedHills:StructBuilder::generateStdTree(vc, treePos);
					break;
				case WoodedBadlands:StructBuilder::generateStdTree(vc, treePos, DryOakTree);
					break;
				case Jungle:StructBuilder::generateStdTree(vc, treePos, SakuraTree);
					break;
				case DarkForest:StructBuilder::generateStdTree(vc, treePos, BirchTree);
					break;
				case Taiga:StructBuilder::generateSpruce(vc, treePos);
					break;
			}

		}

	}

	vc.resetLight();
	vc.recache = true;
	vc.dirty = false; // don't save chunk that can be regenerated

	for (const u8 neighboor: vc.neighboors) {
		if (neighboor) World::chunkSlots[neighboor].recache = true;
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
//    biomeNoise.SetSeed(Random::getSeed());//  Game::getInstance().getSeed());
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

void PerlinWorldGenerator::buildTree(BlockCoord pos, Chunk &vc) {
	s32 height = rand() % 4 + 1;
	//On construit le tronc :
	for (s32 y = 0; y < 3 + height; y++) {
		vc.setBlockType({pos.y + y, pos.x, pos.z}, BlockType::WoodOak);
	}

	//On construit les feuilles :
	for (s32 h = 0; h < 2; h++) {
		for (s32 i = -2; i < 3; i++) {
			for (s32 j = -2; j < 3; j++) {
				if (i == 0 && j == 0) {
					continue;
				}
				if ((i == 2 || i == -2) && (j == 2 || j == -2)) {
					if (rand() % 2)
						vc.setBlockType({pos.y + height + h, pos.x + i, pos.z + j}, BlockType::LeaveOak);
				} else
					vc.setBlockType({pos.y + height + h, pos.x + i, pos.z + j}, BlockType::LeaveOak);
			}
		}
	}
	for (s32 i = -1; i < 2; i++) {
		for (s32 j = -1; j < 2; j++) {
			if (i == 0 && j == 0) {
				continue;
			}
			if ((i == 1 || i == -1) && (j == 1 || j == -1)) {
				if (rand() % 2)
					vc.setBlockType({pos.y + 2 + height, pos.x + i, pos.z + j}, BlockType::LeaveOak);
			} else
				vc.setBlockType({pos.y + 2 + height, pos.x + i, pos.z + j}, BlockType::LeaveOak);
		}
	}

	vc.setBlockType({pos.y + 3 + height, pos.x + 1, pos.z}, BlockType::LeaveOak);
	vc.setBlockType({pos.y + 3 + height, pos.x - 1, pos.z}, BlockType::LeaveOak);
	vc.setBlockType({pos.y + 3 + height,pos.x,  pos.z + 1}, BlockType::LeaveOak);
	vc.setBlockType({pos.y + 3 + height,pos.x,  pos.z - 1}, BlockType::LeaveOak);
	vc.setBlockType({pos.y + 3 + height, pos.x, pos.z}, BlockType::LeaveOak);

}


BiomeType PerlinWorldGenerator::guessBiome(f32 ero, f32 temp, f32 hum, f32 cont, s32 height) {
	// cont < 1.0f || <- falaises
	if (height < Tergen::seaLevel - 1 + Tergen::bottomLevel) { // ===== OCEAN =====
		return BiomeType::Ocean;
	} else if (cont < 1.14f && height < Tergen::seaLevel + Tergen::bottomLevel + 4) {// ===== PLAGE =====
		if (ero > 1.4 && temp < 1.3) { return BiomeType::StonyShore; }
		else if (temp > 1.6f) { return BiomeType::RedBeach; }
		else { return BiomeType::Beach; }
	} else {// ===== CONTINENT =====
		// Cold biomes
		if (temp < .5) {
			if (ero < 1.8) {
				if (hum > 1.) { return Taiga; }
				else { return Tundra; }
			} else { return IcePeak; }
		}
			//Flat hot biomes
		else if (temp > 1.3 && ero < .8) {
			if (hum < .7) { return Desert; }
			else if (hum < 1.4) { return Savanna; }
			else { return Jungle; }
		}
			//hilly hot biomes
		else if (temp > 1.55 && ero >= .8) {
			if (hum < .7) { return Badlands; }
			else if (hum < 1.4) { return WoodedBadlands; }
			return BiomeType::WoodedBadlands;
		}
			//Temperate biomes
		else {
			if (hum < .4) { return StonyLand; }
			else if (hum < 1.) {
				if (ero < 1.) { return Plain; }
				else { return Hills; }
			} else {
				if (ero < .75 && hum > 1.45) { return DarkForest; }
				else if (ero < 1.) { return WoodedPlain; }
				else { return WoodedHills; }
			}


		}
	}
	return BiomeType::Void;
}
