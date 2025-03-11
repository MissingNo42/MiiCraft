#pragma once

#include "cloud.h"
#include "light.h"
#include "skybox.h"
#include "texture.h"

class Renderer;

/// Main data controller
class Environment final {
public:
	TextureControl textureControl{};
	SkyControl skyControl{};
	LightControl lightControl{};
	CloudControl cloudControl{};

	f32 time = 0.5f; // 0=0h, 0.5=12h, 1=24h

	static constexpr f32 timeTick = 0;//1 / (60.f * 60.f);

	consteval Environment() = default;
public:

	void update(const f32 tick = timeTick) {
		time = std::fmod(time + tick, 1.f);
		if (time > ntime(8, 0, 0) && time < ntime(17, 0, 0)) {
			time = ntime(17, 0, 0);
		}
		skyControl.update(time);
		lightControl.update(time);
		cloudControl.update(time);
	}

	friend Renderer;
};
