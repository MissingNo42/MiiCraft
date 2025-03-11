#pragma once

#include <cmath>
#include <gctypes.h>

#include "engine/render/vertex.h"


template <s32 S>
class Icosphere // adapted from https://www.songho.ca/opengl/gl_sphere.html
{
public:
	constexpr static u32 triangleCount = 20 * S * S;
	constexpr static u32 verticesCount = 3 * triangleCount;

	consteval explicit Icosphere(const f32 radius, const u16 offset, Vertex outVertices[verticesCount], TexCoord outTexCoord[verticesCount]) :
		radius(radius),
		offset(offset),
		vertices(outVertices),
		texCoord{outTexCoord} {}

	consteval void load();

private:
	consteval static f32 computeScaleForLength(const f32 v[3], f32 length);

	consteval static void interpolateVertex(const f32 v1[3], const f32 v2[3], f32 alpha, f32 length, f32 newV[3]);

	consteval static void interpolateTexCoord(const f32 t1[2], const f32 t2[2], f32 alpha, f32 newT[2]);

	consteval static f32 lerp(f32 from, f32 to, f32 alpha);

	consteval void computeIcosahedronVertices(f32 initVertices[12 * 3]) const;

	consteval void subdivideVerticesFlat(f32 initVertex[60][3], f32 initTexCoord[60][2]);

	consteval void addVertex(const f32 v[3], const f32 t[2]);

	f32 radius;
	u16 offset;
	u16 vertexIndex = 0;
	Vertex * const vertices;
	TexCoord * const texCoord;
};


template <s32 S>
consteval void Icosphere<S>::computeIcosahedronVertices(f32 initVertices[12 * 3]) const {
	const f32 PI      = acos(-1.0f);
	const f32 H_ANGLE = PI / 180 * 72;    // 72 degree = 360 / 5
	const f32 V_ANGLE = atanf(1.0f / 2);  // elevation = 26.565 degree

	s32 i1;
	f32 hAngle1 = -PI / 2 - H_ANGLE / 2;  // start from -126 deg at 2nd row
	f32 hAngle2 = -PI / 2;                // start from -90 deg at 3rd row

	// the first top vertex (0, 0, r)
	initVertices[0] = 0;
	initVertices[1] = 0;
	initVertices[2] = radius;

	// 10 vertices at 2nd and 3rd rows
	for (s32 i = 1; i <= 5; ++i) {
		i1     = i * 3;         // for 2nd row
		s32 i2 = (i + 5) * 3;   // for 3rd row

		const f32 z  = radius * sinf(V_ANGLE); // elevaton
		const f32 xy = radius * cosf(V_ANGLE);

		initVertices[i1++] = xy * cosf(hAngle1);  // x
		initVertices[i1++] = xy * sinf(hAngle1);  // y
		initVertices[i1]   = z;                   // z
		initVertices[i2++] = xy * cosf(hAngle2);
		initVertices[i2++] = xy * sinf(hAngle2);
		initVertices[i2]   = -z;

		// next horizontal angles
		hAngle1 += H_ANGLE;
		hAngle2 += H_ANGLE;
	}

	// the last bottom vertex (0, 0, -r)
	i1                 = 11 * 3;
	initVertices[i1++] = 0;
	initVertices[i1++] = 0;
	initVertices[i1]   = -radius;
}

template <s32 S>
consteval void Icosphere<S>::load() {
	constexpr f32 S_STEP = 186 / 2048.0f;     // horizontal texture step
	constexpr f32 T_STEP = 322 / 1024.0f;     // vertical texture step

	// compute 12 vertices of icosahedron
	s32 idx = 0;
	f32 tmpVertices[12 * 3];
	f32 initVertex[60][3];
	f32 initTexCoord[60][2];
	computeIcosahedronVertices(tmpVertices);

	const f32 * v2, * v4;          // vertex positions
	f32 t0[2], t1[2], t2[2], t3[2], t4[2], t11[2];    // texCoords

	// compute 20 tiangles of icosahedron first
	const f32 * v0  = &tmpVertices[0];       // 1st vertex (north pole)
	const f32 * v11 = &tmpVertices[11 * 3]; // 12th vertex (south pole)
	for (s32 i = 1; i <= 5; ++i) {
		// 4 vertices in the 2nd row
		const f32 * v1 = &tmpVertices[i * 3];
		if (i < 5) v2 = &tmpVertices[(i + 1) * 3];
		else v2       = &tmpVertices[3];

		const f32 * v3 = &tmpVertices[(i + 5) * 3];
		if ((i + 5) < 10) v4 = &tmpVertices[(i + 6) * 3];
		else v4              = &tmpVertices[6 * 3];

		// texture coords
		t0[0]  = (2 * i - 1) * S_STEP;
		t0[1]  = 0;
		t1[0]  = (2 * i - 2) * S_STEP;
		t1[1]  = T_STEP;
		t2[0]  = (2 * i - 0) * S_STEP;
		t2[1]  = T_STEP;
		t3[0]  = (2 * i - 1) * S_STEP;
		t3[1]  = T_STEP * 2;
		t4[0]  = (2 * i + 1) * S_STEP;
		t4[1]  = T_STEP * 2;
		t11[0] = 2 * i * S_STEP;
		t11[1] = T_STEP * 3;

		#define newVertex(v, t) \
		initVertex[idx][0] = v[0]; \
		initVertex[idx][1] = v[1]; \
		initVertex[idx][2] = v[2]; \
		initTexCoord[idx][0] = t[0]; \
		initTexCoord[idx++][1] = t[1];

		// add a triangle in 1st row
		newVertex(v0, t0);
		newVertex(v1, t1);
		newVertex(v2, t2);

		// add 2 triangles in 2nd row
		newVertex(v1, t1);
		newVertex(v3, t3);
		newVertex(v2, t2);

		newVertex(v2, t2);
		newVertex(v3, t3);
		newVertex(v4, t4);

		// add a triangle in 3rd row
		newVertex(v3, t3);
		newVertex(v11, t11);
		newVertex(v4, t4);
	}

	// subdivide icosahedron
	subdivideVerticesFlat(initVertex, initTexCoord);
}

template <s32 S>
consteval void Icosphere<S>::subdivideVerticesFlat(f32 initVertex[60][3], f32 initTexCoord[60][2]) {
	if constexpr (S <= 1) {
		for (s32 i = 0; i < 60; ++i) {
			addVertex(initVertex[i], initTexCoord[i]);
		}
		return;
	}


	constexpr u32 subVertexCount = (S + 1) * (S + 2) / 2;

	s32 j, k;
	// lerp alpha

	// copy prev arrays

	for (s32 i = 0; i < 60; i += 3) {
		// get 3 vertice and texcoords of a triangle of icosahedron
		const f32 * v1 = initVertex[i];
		const f32 * v2 = initVertex[i + 1];
		const f32 * v3 = initVertex[i + 2];
		const f32 * t1 = initTexCoord[i];
		const f32 * t2 = initTexCoord[i + 1];
		const f32 * t3 = initTexCoord[i + 2];

		// add top vertex and textcoord (x,y,z), (s,t)
		s32 newVertexIndex   = 0;
		s32 newTexCoordIndex = 0;

		f32 newVs[subVertexCount * 3];
		f32 newTs[subVertexCount * 2];

		newVs[newVertexIndex++] = v1[0];
		newVs[newVertexIndex++] = v1[1];
		newVs[newVertexIndex++] = v1[2];

		newTs[newTexCoordIndex++] = t1[0];
		newTs[newTexCoordIndex++] = t1[1];

		// find new vertices by subdividing edges
		for (j = 1; j <= S; ++j) {
			f32 newT2[2];
			f32 newT1[2];
			f32 newV2[3];
			f32 newV1[3];
			f32 a = static_cast<f32>(j) / S;    // lerp alpha

			// find 2 end vertices on the edges of the current row
			//          v1           //
			//         / \           // if N = 3,
			//        *---*          // lerp alpha = 1 / N
			//       / \ / \         //
			// newV1*---*---* newV2  // lerp alpha = 2 / N
			//     / \newV3/ \       //
			//    v2--*---*---v3     //
			interpolateVertex(v1, v2, a, radius, newV1);
			interpolateVertex(v1, v3, a, radius, newV2);
			interpolateTexCoord(t1, t2, a, newT1);
			interpolateTexCoord(t1, t3, a, newT2);

			newVs[newVertexIndex++] = newV1[0];
			newVs[newVertexIndex++] = newV1[1];
			newVs[newVertexIndex++] = newV1[2];

			newTs[newTexCoordIndex++] = newT1[0];
			newTs[newTexCoordIndex++] = newT1[1];

			for (k = 1; k < j; ++k) {
				f32 newT3[2];
				f32 newV3[3];
				a = static_cast<f32>(k) / j;
				interpolateVertex(newV1, newV2, a, radius, newV3);
				interpolateTexCoord(newT1, newT2, a, newT3);

				newVs[newVertexIndex++] = newV3[0];
				newVs[newVertexIndex++] = newV3[1];
				newVs[newVertexIndex++] = newV3[2];

				newTs[newTexCoordIndex++] = newT3[0];
				newTs[newTexCoordIndex++] = newT3[1];
			}

			newVs[newVertexIndex++] = newV2[0];
			newVs[newVertexIndex++] = newV2[1];
			newVs[newVertexIndex++] = newV2[2];

			newTs[newTexCoordIndex++] = newT2[0];
			newTs[newTexCoordIndex++] = newT2[1];
		}

		// compute sub-triangles from new vertices
		//      /           //
		//   V1*---*-       // prev row
		//    / \ /         //
		// V2*---*V3-       // curr row
		//  /               //
		for (j = 1; j <= S; ++j) {
			for (k = 0; k < j; ++k) {
				// indices
				const u32 i1 = (j - 1) * j / 2 + k;   // index from prev row
				u32 i2       = j * (j + 1) / 2 + k;   // index from curr row

				v1 = &newVs[i1 * 3];
				v2 = &newVs[i2 * 3];
				v3 = &newVs[(i2 + 1) * 3];

				t1 = &newTs[i1 * 2];
				t2 = &newTs[i2 * 2];
				t3 = &newTs[(i2 + 1) * 2];

				addVertex(v1, t1);
				addVertex(v2, t2);
				addVertex(v3, t3);

				// if K is not the last, add adjacent triangle
				if (k < (j - 1)) {
					i2 = i1 + 1; // next of the prev row
					v2 = &newVs[i2 * 3];
					t2 = &newTs[i2 * 2];

					addVertex(v1, t1);
					addVertex(v3, t3);
					addVertex(v2, t2);
				}
			}
		}
	}
}

template <s32 S>
consteval void Icosphere<S>::addVertex(const f32 v[3], const f32 t[2]) {
	vertices[vertexIndex].x  = v[0];
	vertices[vertexIndex].y  = v[1];
	vertices[vertexIndex].z  = v[2];
	vertices[vertexIndex].tc = offset + vertexIndex;

	texCoord[vertexIndex].u = t[0];
	texCoord[vertexIndex].v = t[1];

	vertexIndex++;
}


template <s32 S>
consteval f32 Icosphere<S>::computeScaleForLength(const f32 v[3], const f32 length) {
	// and normalize the vector then re-scale to new radius
	f32 result  = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

	const f32 x = result;

	double delta;
	do { // sqrt
		const double previous = result;
		result                = 0.5 * (previous + x / previous);
		delta                 = result - previous;
	} while (delta > 1e-10 || delta < -1e-10);

	return length / result;
}

template <s32 S>
consteval void Icosphere<S>::interpolateVertex(const f32 v1[3], const f32 v2[3], const f32 alpha, const f32 length, f32 newV[3]) {
	newV[0]         = lerp(v1[0], v2[0], alpha);
	newV[1]         = lerp(v1[1], v2[1], alpha);
	newV[2]         = lerp(v1[2], v2[2], alpha);
	const f32 scale = computeScaleForLength(newV, length);
	newV[0] *= scale;
	newV[1] *= scale;
	newV[2] *= scale;
}

template <s32 S>
consteval void Icosphere<S>::interpolateTexCoord(const f32 t1[2], const f32 t2[2], const f32 alpha, f32 newT[2]) {
	newT[0] = lerp(t1[0], t2[0], alpha);
	newT[1] = lerp(t1[1], t2[1], alpha);
}

template <s32 S>
consteval f32 Icosphere<S>::lerp(const f32 from, const f32 to, const f32 alpha) {
	return from + alpha * (to - from);
}
