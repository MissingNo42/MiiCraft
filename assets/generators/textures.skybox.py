#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Distort the skybox textures to create a cubemap.
"""

import math as m
import os
import sys
import random
from typing import Callable
from dataclasses import dataclass

import cv2
import numpy as np

TEX_NIGHT_SZ = 768
TEX_SZ = 512
TILE_SIZE = 32


class TiledTexture:
    """
    Represents a texture that can be tiled in a grid.
    """

    def __init__(self, size: int, tile: int, idx2pos: Callable[[int, int], tuple[int, int]], channel: int = 4):
        self.size = size
        self.tile = tile
        self.idx2pos = idx2pos
        self.channel = channel
        self.texture = np.zeros((size, size, channel), dtype=np.uint8)
        self.idx = 0

    def get_pos(self, idx: int = None) -> tuple[int, int]:
        """
        Get the position of a tile in the texture.

        Args:
            idx: the index of the tile (if None, the next index is used)

        Returns: the top-left position of the tile
        """

        if idx is None:
            idx = self.idx
            self.idx += 1

        return self.idx2pos(idx, self.tile)


@dataclass
class File:
    """
    Represents a texture to process, how to process it and where to put it in the output texture.
    """
    name: str
    tex: TiledTexture
    tiles: list[tuple[int, int, Callable[[np.ndarray, int, int, int], np.ndarray]]]
    file: str = ""


NIGHT_TEXTURE = TiledTexture(TEX_NIGHT_SZ, TEX_NIGHT_SZ, lambda idx, tl: (0, 0), 3)
SKY_TEXTURE = TiledTexture(TEX_SZ, TILE_SIZE, lambda idx, tl: ((TEX_SZ >> 1) + ((-tl * (idx + 1)) & 0xff),
                                                               TEX_SZ - (idx // ((TEX_SZ >> 1) // tl) + 1) * tl))


def clamp(x: float, low: float, high: float) -> float:
    """
    Clamp a value between a low and high value.

    Args:
        x: the value to clamp
        low: the lower bound
        high: the upper bound

    Returns: the clamped value
    """

    return max(low, min(x, high))


# Misc texturing interpolation functions


def texturing_interpolation_wrap(arr: np.ndarray, y: int, x: int, sz: int) -> np.ndarray:
    """
    Get a color from the texture array with wrap coordinates correction.

    Args:
        arr: the texture array
        y: the y coordinate
        x: the x coordinate
        sz: the size of the texture

    Returns: the color at the given coordinates
    """

    return arr[y % sz, x % sz]


def texturing_interpolation_mirror(arr: np.ndarray, y: int, x: int, sz: int) -> np.ndarray:
    """
    Get a color from the texture array with mirror coordinates correction.

    Args:
        arr: the texture array
        y: the y coordinate
        x: the x coordinate
        sz: the size of the texture

    Notes:
        - multiple mirror-bouncing not supported (e.g. means -2 sz < x < 2 sz)

    Returns: the color at the given coordinates
    """

    ox, oy = x, y
    x = abs(x)
    y = abs(y)

    if x >= sz:
        x = 2 * sz - 1 - x

    if y >= sz:
        y = 2 * sz - 1 - y

    r = arr[y, x]

    if ox != x or oy != y:  # correction reuse the border alpha on mirroring
        r[3] = arr[clamp(oy, 0, sz - 1), clamp(ox, 0, sz - 1), 3]

    return r


def texturing_interpolation_mirror_l(arr: np.ndarray, y: int, x: int, sz: int) -> np.ndarray:
    """
    Get a color from the texture array with left mirror coordinates correction.

    Args:
        arr: the texture array
        y: the y coordinate
        x: the x coordinate
        sz: the size of the texture

    Notes:
        - multiple mirror-bouncing not supported (e.g. means -2 sz < x < 2 sz)
        - 'repeat' correction on the right side (stretching of the last column)

    Returns: the color at the given coordinates
    """

    ox, oy = x, y
    x = min(abs(x), sz - 1)  # clamp
    y = abs(y)

    if y >= sz:
        y = 2 * sz - 1 - y

    r = arr[y, x]

    if ox != x or oy != y:  # correction reuse the border alpha
        r[3] = arr[clamp(oy, 0, sz - 1), clamp(ox, 0, sz - 1), 3]

    return r


def texturing_interpolation_mirror_r(arr: np.ndarray, y: int, x: int, sz: int) -> np.ndarray:
    """
    Get a color from the texture array with right mirror coordinates correction.

    Args:
        arr: the texture array
        y: the y coordinate
        x: the x coordinate
        sz: the size of the texture

    Notes:
        - multiple mirror-bouncing not supported (e.g. means -2 sz < x < 2 sz)
        - 'repeat' correction on the left side (stretching of the last column)

    Returns: the color at the given coordinates
    """

    ox, oy = x, y
    x = max(x, 0)
    y = abs(y)

    if x >= sz:
        x = 2 * sz - 1 - x

    if y >= sz:
        y = 2 * sz - 1 - y

    r = arr[y, x]

    if ox != x or oy != y:  # correction reuse the border alpha
        r[3] = arr[clamp(oy, 0, sz - 1), clamp(ox, 0, sz - 1), 3]

    return r


def texturing_interpolation_mirror_s(arr: np.ndarray, y: int, x: int, sz: int) -> np.ndarray:
    """
    Get a color from the texture array with horizontal mirror coordinates correction.

    Args:
        arr: the texture array
        y: the y coordinate
        x: the x coordinate
        sz: the size of the texture

    Notes:
        - multiple mirror-bouncing not supported (e.g. means -2 sz < x < 2 sz)
        - 'repeat' correction on vertical side (stretching of the extreme rows)

    Returns: the color at the given coordinates
    """

    ox, oy = x, y
    x = abs(x)
    y = clamp(y, 0, sz - 1)

    if x >= sz:
        x = 2 * sz - 1 - x

    return arr[y, x]


# Configuration of the files to process and output textures


FILES = (
    FILE_NIGHT := File("NIGHT", NIGHT_TEXTURE,
                       [(0, 0, texturing_interpolation_wrap, "TBNSWE")]),
    FILE_BLUE := File("DAY", SKY_TEXTURE,
                      [(2, 0, texturing_interpolation_mirror, "NSWE"),
                       (None, None, (0, .5, 0), "T"),  # use pixel (W.5, H0) from the first tile (0) for the top (full blue)
                       (None, None, (0, .5, 1), "B")   # use pixel (W.5, H1) from the first tile (0) for the bottom (full alpha)
                       ]),
    FILE_SUNRISE := File("SUNRISE", SKY_TEXTURE,
                         [(0, 1, texturing_interpolation_mirror_s, "NSWE"),
                          (None, None, (0, .5, 0), "T"),  # use pixel (W.5, H0) from the first tile (0) for the top
                          (None, None, (0, .5, 1), "B")   # use pixel (W.5, H1) from the first tile (0) for the bottom
                          ]),
    FILE_SUNSET := File("SUNSET", SKY_TEXTURE,
                        FILE_BLUE.tiles),
    FILE_SUNRISE_E := File("SUNRISE_EFFECT", SKY_TEXTURE,
                           [(2, 0, texturing_interpolation_mirror_l, "S"),
                            (1, 1, texturing_interpolation_mirror_r, "N"),
                            (2, 1, texturing_interpolation_mirror, "E"),
                            (None, None, (2, .5, 1), "TBW")
                            ]),
    FILE_SUNSET_E := File("SUNSET_EFFECT", SKY_TEXTURE,
                          [(2, 0, texturing_interpolation_mirror_r, "S"),
                           (1, 1, texturing_interpolation_mirror_l, "N"),
                           (0, 1, texturing_interpolation_mirror, "W"),
                           (None, None, (2, .5, 1), "TBE")]),
)


# Math coordinates distorsion functions

def sq2sp(u: float, v: float, r=2 ** .5) -> tuple[float, float]:  # translate square 2x2 to a circle of radius r
    if u == 0 == v:
        return 0., 0.

    k = 1 / (u ** 2 + v ** 2) ** .5
    n = r * k * max(abs(u), abs(v))

    return u * n, v * n


def sp2sq(u: float, v: float, r=2 ** .5) -> tuple[float, float]:  # translate a circle of radius r to a square 2x2
    if abs(u) > abs(v):
        n = r * 1 / (1 + v ** 2 / u ** 2) ** .5
        x = u / n
        return x, v / u * x

    elif abs(u) == abs(v):
        n = r / 2 ** .5
        x = u / n
        return x, x

    n = r * 1 / (1 + u ** 2 / v ** 2) ** .5
    y = v / n

    return u / v * y, y


def resize(u: float, v: float, k: float) -> tuple[float, float]:  # resize by a factor k
    return u * k, v * k


def transform(u: float, v: float) -> tuple[float, float]:  # perfect but crop
    q = (3 - u ** 2 - v ** 2) ** .5
    return u / q, v / q


def rtransform(u: float, v: float) -> tuple[float, float]:  # perfect but crop (reciproque of transform)
    q = (3 / (1 + u ** 2 + v ** 2)) ** .5
    return u * q, v * q


def distort_coords(x: int, y: int, size: int) -> tuple[float, float]:  # distorsion of the coordinates
    """
    Distort the coordinates of a pixel in a texture.
    The applied distortion is the projection of a plane (a cube face) on a sphere.
    The idea is to generate a cubemap using "flat"/not distorded textures,
    by projecting the 6 "faces" of a sphere to its inner cube.

    Args:
        x: the x coordinate in [0, size[
        y: the y coordinate in [0, size[
        size: the size of the texture

    Returns: the distorted coordinates

    Notes:
        - the distorded coordinates are returned as floating point values for better precision and interpolation
        - the distorded coordinates are in the same space as the input coordinates,
          but can be outside the [0, size[ range, needing interpolation for creating the missing data
        - the distorsion is thus not perfect due to this needed interpolation, but it has the property to keep lines,
          this means that connected contiguous lines will result as a surrouding circle when cubemapped
        - a perfect distorsion (that needs no interpolation) may exists, but wasn't found yet
    """

    # Normalize coordinates to range [-1, 1]
    u = 2 * (x + .5) / size - 1
    v = 2 * (y + .5) / size - 1

    du, dv = rtransform(u, v)  # need texturing correction to create the missing data
    # PROJ_PS_MAX = 1.5 ** .5
    # du, dv = resize(*rtransform(u, v), 1 / PROJ_PS_MAX) # effect ok but crop
    # du, dv = m.sin(u * m.pi/2), m.sin(v * m.pi/2)
    # du, dv = resize(*sp2sq(*alt(*sq2sp(u, v))), 1.5)

    # du, dv = sp2sq(*rtransform(*resize(u, v, 1))) # effect ok but lines lost
    # du, dv = sp2sq(*rtransform(*sp2sq(u, v))) # effect ok but lines lost
    # du, dv = sp2sq(*rtransform(*sq2sp(u, v))) # effect ok but lines lost
    # du, dv = sq2sp(*rtransform(*sp2sq(u, v))) # bad effect
    # du, dv = sq2sp(*rtransform(*sq2sp(u, v))) # bad effect
    # du, dv = resize(*sq2sp(u, v), .5)
    # du, dv = resize(*sp2sq(u, v), .5)
    # du, dv = sp2sq(*rtransform(u, v))
    # du, dv = resize(*sq2sp(*rtransform(u, v)),0.5)
    # du, dv = resize(*rtransform(*sp2sq(u, v)), .8)
    # du, dv = resize(*rtransform(*sq2sp(u, v)), .5)
    # may trying with a variant of sX2sY that is based on sphere rather than circle (2D -> 3D)

    # Convert back to pixel coordinates
    dx = (du + 1) * size / 2
    dy = (dv + 1) * size / 2

    return dx, dy


def bilinear_interpolate(arr: np.ndarray, y: float, x: float, sz: int,
                         texint: Callable[[np.ndarray, int, int, int], np.ndarray]) -> np.ndarray:
    """
    Bilinear interpolation of a color from the texture array.

    Args:
        arr: the texture array
        y: the y coordinate
        x: the x coordinate
        sz: the size of the texture
        texint: the texture interpolation function, that create the missing data

    Returns: the interpolated color
    """

    x0 = m.floor(x)
    x1 = x0 + 1
    y0 = m.floor(y)
    y1 = y0 + 1

    pa = texint(arr, y0, x0, sz)
    pb = texint(arr, y1, x0, sz)
    pc = texint(arr, y0, x1, sz)
    pd = texint(arr, y1, x1, sz)

    wa = (x1 - x) * (y1 - y)
    wb = (x1 - x) * (y - y0)
    wc = (x - x0) * (y1 - y)
    wd = (x - x0) * (y - y0)

    r = np.round(wa * pa + wb * pb + wc * pc + wd * pd).astype(np.uint8)

    return r


def correct(tile: np.ndarray, corrected: np.ndarray,
            texint: Callable[[np.ndarray, int, int, int], np.ndarray]) -> None:
    """
    Apply a correction from the tile texture, to the corrected texture.
    Args:
        tile: the texture tile to correct
        corrected: the corrected texture
        texint: the texture correction function
    """

    S = tile.shape[0]

    for y in range(S):

        for x in range(S):
            dx, dy = distort_coords(x, y, S)
            corrected[y, x] = bilinear_interpolate(tile, dy - .5, dx - .5, S, texint)
            # corrected[dy, dx] = tile[y, x]  # reciproque testing (cause artifacts so debug only)

            # DEBUG
            # Y, X = round(dy - .5), round(dx - .5)
            # try:
            #     if Y < 0 or X < 0: raise IndexError
            #     corrected[y, x] = tile[Y, X] * .5
            # except IndexError:  # apply mirror correction
            #     #corrected[y, x] = (0, 0, 255, 255)
            #     X = abs(X)
            #     Y = abs(Y)
            #     if X >= S: X = 2 * S - 1 - X
            #     if Y >= S: Y = 2 * S - 1 - Y
            #     try:
            #         corrected[y, x] = tile[Y, X]
            #     except IndexError:
            #         corrected[y, x] = (0, 0, 255, 255)


def process(file: File) -> str:
    """
    Extract and process the texture from a file.

    Args:
        file: the file to process

    Returns:
        the header data payload of the texture
    """

    print("processing", file.name, "texture...")

    assert set(''.join(i[-1] for i in file.tiles)) == set("TBNSWE")

    img = cv2.imread(file.file, cv2.IMREAD_UNCHANGED)[:, :, :file.tex.channel]

    H = img.shape[0]
    W = img.shape[1]
    S = H >> 1

    assert S * 3 == W, (S, H, W)

    payload = ""
    coords = []

    for n, (x, y, texint, face) in enumerate(file.tiles):
        if x is None:  # special case where flat uniform texture just recycle 1 other texture's pixel
            target, x, y = texint
            px, py = coords[target]
            pad = 2  # padding to avoid interpolation artifacts caused by GPU reading on border
            px += round(clamp(x * file.tex.tile, pad, file.tex.tile - pad))
            py += round(clamp(y * file.tex.tile, pad, file.tex.tile - pad))
            sz = 1

        else:
            tile = img[S * y: S * (y + 1), S * x: S * (x + 1)]
            corrected = np.zeros((S, S, file.tex.channel), dtype=np.uint8)
            correct(tile, corrected, texint)

            px, py = file.tex.get_pos()
            sz = file.tex.tile
            file.tex.texture[py: py + sz, px: px + sz] = cv2.resize(corrected, (sz, sz), interpolation=cv2.INTER_AREA)

        coords.append((px, py))

        for f in face:
            payload += f"inline constexpr u16 CUBEMAP_{file.name}_{f}_X = {px};\n"
            payload += f"inline constexpr u16 CUBEMAP_{file.name}_{f}_Y = {py};\n"
            payload += f"inline constexpr u16 CUBEMAP_{file.name}_{f}_SZ = {sz};\n\n"

    return payload


def generate(files: tuple[str], main_out: str, night_out: str, header_out: str) -> None:
    """
    Generate the cubemap textures.

    Args:
        files: the files to process
        main_out: the main output texture
        night_out: the night output texture
        header_out: the header output file
    """

    payload = ""

    for n, file in enumerate(FILES):
        # if n != 0: continue  # debug

        file.file = next(i for i in files if file.name == os.path.splitext(os.path.basename(i))[0].upper().replace(".", "_").replace("_DEBUG", ""))
        payload += process(file)

    cv2.imwrite(main_out, SKY_TEXTURE.texture, [cv2.IMWRITE_PNG_COMPRESSION, 0])
    cv2.imwrite(night_out, NIGHT_TEXTURE.texture, [cv2.IMWRITE_PNG_COMPRESSION, 0])

    with open(header_out, "w") as f:
        f.write(payload)

    # tl = cv2.imread('../texture-tl.png', cv2.IMREAD_UNCHANGED)
    # tl += SKY_TEXTURE.texture  # blend with the main texture

    # cv2.imwrite('../texture.png', tl, [cv2.IMWRITE_PNG_COMPRESSION, 0])


if __name__ == '__main__':
    _, *files, main_out, night_out, header_out = sys.argv
    generate(files, main_out, night_out, header_out)

    # cv2.imshow('Result Night', NIGHT_TEXTURE.texture)
    # cv2.imshow('Result', SKY_TEXTURE.texture)
    # cv2.imshow('Result Alpha', cv2.split(SKY_TEXTURE.texture)[3])

    # cv2.waitKey(0)
    # cv2.destroyAllWindows()
