//
// Created by Romain on 05/02/2024.
//

#ifndef MIICRAFT_CAMERA_H
#define MIICRAFT_CAMERA_H

#include <gccore.h>
#include <ogc/gu.h>
//#include "wiimote.h"
#include  "world/world.h"
#include "utils/matrix.h"

class Camera {

public:
	Mtx44 perspective;
    Mtx viewMatrix;
    guVector up = {0.0F, 1.0F, 0.0F};
	guVector pos = {0.0F, 0.0F, 0.0F},
    look = {0.0F, 0.0F, 1.0F};

	/** Constructor
	 * @param fov Field of view (half angle in degrees)
	 * @param min Minimum render distance
	 * @param max Maximum render distance
	 */
	explicit Camera(f32 fov = 45, f32 min = .1, f32 max = 300);
	
	~Camera() = default;

	void update(bool applyTransform);
	
	void applyTransform();
	
	void rotateH(f32 rad);
	
	void rotateV(f32 rad);

};


#endif //MIICRAFT_CAMERA_H
