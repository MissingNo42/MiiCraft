#pragma once
#include <gctypes.h>

/*

The rasterization of interpolated vertex colors on quads produces artefacts on Wii when in some config like 3 dark corners and 1 bright corner.
These are caused by the fact that quads are just 2 triangles and the rasterizer interpolates the colors linearly in each triangle.
The solution is to reverse (or shift) the order of the vertices in the quads so that the interpolation can operate on a wider range of colors.
Using LightContact.contact, these lookup tables says if a quad should be reversed or not.

Python code to generate the following tables:

gG = gA = lambda x: (x >> 6) & 3
gH = gB = lambda x: (x >> 4) & 3
gE = gC = lambda x: (x >> 2) & 3
gF = gD = lambda x: (x >> 0) & 3

def cx(c1, c2, c3, c4):
    d1 = abs(c1 - c3)  # "default" quad diagonal (triangle1 = c1, c2, c3, triangle2 = c1, c3, c4), see GX reference 4.4.1 about GX_QUADS
    d2 = abs(c2 - c4)  # the "other" diagonal

    if d1 == d2:  # if equals, privilegiate the diagonal with the highest value
        m1 = max(c1, c3)
        m2 = max(c2, c4)

        if m1 == m2:  # if equals, privilegiate the diagonal with the lowest value
            m1 = min(c1, c3)
            m2 = min(c2, c4)

        return m1 < m2

    return d1 < d2  # if true, high contrast on d2 diagonal so reverse the quad

def cv(i, q):
    return eval("cx(g{0}(i), g{1}(i), g{2}(i), g{3}(i))".format(*q))

for n, i in enumerate([cv(i, "CDAB") for i in range(0x100)]): print("" if n%16 else "\n", f"{i},", end="")  # for TB table
 */

extern const bool reverseQuadTB[0x100];
extern const bool reverseQuadLR[0x100];
extern const bool reverseQuadFK[0x100];

// shifted space from 16x16 (4-bits) to 64x64 (6-bits)
// color idx in 'Lights' array is the sum of at most 4 extendedLight's values:
// extendedLight[block.light = 0bnnnnaaaa] = 0b0000_00(nnnn + 1)_00(aaaa + 1) // the +1 is to apply a bonus on non-occluded lights
extern const u16 extendedLight[0x100];

// shifted back space from 64x64 (6-bits) to 16x16 (4-bits)
extern const u8 reducedLight[0x1000];

/**
 * Get the (extended) ambient occlusion light correction for a given light value to add to the light value, giving the current (reduced) light value
 * and the number of non-occluded neighboors (0 to 2)
 **/
extern const u16 correctedLight[2][3][0x100];
