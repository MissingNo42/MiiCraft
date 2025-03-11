#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Exports the cloud data texture as vertices to a C++ file.
"""

from __future__ import annotations  # Py 3.12

import sys

from math import floor
from tqdm import tqdm
from typing import Any, Self, Iterator

import numpy as np
import cv2

assert sys.version_info >= (3, 12), f"Python 3.12+ required, got {sys.version_info}"

type Pixel = tuple[int, int]  # represents the coordinate of a pixel
type Vertex = tuple[int, int]  # represents the coordinate of a vertex
type Vertex3D = tuple[int, int, int]  # represents the coordinate of a vertex
type Shape = set[Pixel]  # represents a cloud shape (all the pixels of a 2-axis contiguous cloud)
type Border = set[Pixel]  # represents a cloud border (all the pixels at the border of a shape, outer AND inners)
type BorderVertices = set[Vertex]  # represents the vertices of a cloud border (outer AND inners)
type Ring = list[Vertex]  # represents the vertices of a cloud border (ordered, outer OR inner)
type ShapeRings = list[Ring]  # represents the rings of a cloud shape (outer first AND inners then)
type Line = list[Vertex]  # represents the vertices of a cloud border (ordered, outer AND inner)
type Triangle = tuple[Vertex, Vertex, Vertex]  # represents a triangle (3 vertices)
type Triangle3D = tuple[Vertex3D, Vertex3D, Vertex3D]  # represents a triangle (3 vertices)
type Polygon = list[Triangle]  # represents a polygon (a list of triangles)
type Polygon3D = list[Triangle3D]  # represents a polygon (a list of triangles)
type Edge = frozenset[Vertex, Vertex]


class IOProxy:
    """
    A proxy for an IO stream that removes newlines from the output (tqdm x clion patch)
    """

    def __init__(self, file):
        self._file = file

    def write(self, s) -> None:
        s = s.replace("\n", "")
        self._file.write(s)

    def __getattr__(self, item: str) -> Any:
        return getattr(self._file, item)


stdout = IOProxy(sys.stdout)


def propagate(shape: Shape, x: int, y: int, cloud: np.array) -> None:
    """
    [Internal] Propagate a cloud shape from a pixel to its neighbors.

    Args:
        shape: the cloud shape
        x: the x-coordinate of the pixel to explore
        y: the y-coordinate of the pixel to explore
        cloud: the cloud flat image
    """

    if x < 0 or y < 0 or x >= cloud.shape[0] or y >= cloud.shape[1] or not cloud[x, y]:
        return

    cloud[x, y] = False
    shape.add((x, y))

    propagate(shape, x + 1, y, cloud)
    propagate(shape, x - 1, y, cloud)
    propagate(shape, x, y + 1, cloud)
    propagate(shape, x, y - 1, cloud)


def extract_shapes(cloud: np.array) -> list[Shape]:
    """
    Extract the different cloud shapes from a cloud flat image.

    Args:
        cloud: the cloud flat image

    Returns: a list of shapes, each shape is a set of pixels coordinates
    """

    shapes = []

    for x, y in tqdm(((x, y) for x in range(cloud.shape[0]) for y in range(cloud.shape[1])), desc="Extracting shapes\t",
                     total=cloud.shape[0] * cloud.shape[1], file=stdout, ncols=120, unit=" pixel"):
        if cloud[x, y]:
            shapes.append(shape := set())
            propagate(shape, x, y, cloud)

    return shapes


def extract_borders(shapes: list[Shape]) -> list[Border]:
    """
    Extract the borders of the different cloud shapes.

    Args:
        shapes: the list of shapes

    Returns: a list of borders, each border is a set of pixels coordinates
    """

    borders = []

    for shape in tqdm(shapes, desc="Extracting borders\t", file=stdout, ncols=120, unit=" cloud"):
        border = set()

        for x, y in shape:

            if (x + 1, y) not in shape or (x - 1, y) not in shape or (x, y + 1) not in shape or (x, y - 1) not in shape:
                border.add((x, y))

        borders.append(border)
    return borders


def extract_vertices(shapes: list[Shape], borders: list[Border]) -> list[BorderVertices]:
    """
    Extract the vertices of the different cloud borders.

    Args:
        shapes: the list of shapes
        borders: the list of borders

    Returns: a list of borders' vertices, each border's vertices is a set of pixels coordinates
    """

    vertices = []

    for bd, shape in tqdm(zip(borders, shapes), total=len(borders), desc="Extracting vertices\t", file=stdout,
                          ncols=120, unit=" cloud"):
        vertex = set()

        for x, y in bd:

            if (x + 1, y) not in shape:
                vertex.add((x + 1, y))
                vertex.add((x + 1, y + 1))
            if (x - 1, y) not in shape:
                vertex.add((x, y))
                vertex.add((x, y + 1))
            if (x, y + 1) not in shape:
                vertex.add((x, y + 1))
                vertex.add((x + 1, y + 1))
            if (x, y - 1) not in shape:
                vertex.add((x, y))
                vertex.add((x + 1, y))

        vertices.append(vertex)
    return vertices


def split_vertices(vertices: list[BorderVertices], shapes: list[Shape]) -> list[ShapeRings]:
    """
    Split the vertices of the different cloud borders into rings.

    Args:
        vertices: the list of borders' vertices
        shapes: the list of shapes

    Returns: a list of borders' rings, each border's rings is a list of rings, each ring is a list of vertices
    """

    rings = []

    for n, (vt, sh) in tqdm(enumerate(zip(vertices, shapes)), desc="Splitting vertices\t", total=len(vertices),
                            file=stdout, ncols=120, unit=" cloud"):
        unprocessed = vt.copy()
        ring = [r := {unprocessed.pop()}]

        while unprocessed:
            processed = set()
            for x, y in unprocessed:
                neighbors = set()

                if ((x, y) in sh) != ((x, y - 1) in sh):  # check border (x, y)::(x + 1, y)
                    neighbors.add((x + 1, y))

                if ((x, y) in sh) != ((x - 1, y) in sh):  # check border (x, y)::(x, y + 1)
                    neighbors.add((x, y + 1))

                if ((x - 1, y) in sh) != ((x - 1, y - 1) in sh):  # check border (x, y)::(x - 1, y)
                    neighbors.add((x - 1, y))

                if ((x, y - 1) in sh) != ((x - 1, y - 1) in sh):  # check border (x, y)::(x, y - 1)
                    neighbors.add((x, y - 1))

                if neighbors & r:
                    r.add((x, y))
                    processed.add((x, y))

            if not processed:
                ring.append(r := {unprocessed.pop()})  # start a new ring
            else:
                unprocessed -= processed

        ring.sort(key=lambda rg: min(rg, key=lambda cd: cd[0])[0])  # ring[0] = outer ring
        rcs = []

        for r in ring:  # reconstruct rings vertex order
            rx = r.copy()
            rc = [ls := rx.pop()]
            lls = None
            while rx:
                x, y = ls

                neighbors = set()
                distants = set()

                if (((x, y) in sh) != ((x, y - 1) in sh)) and (x + 1, y) in rx:  # check border (x, y)::(x + 1, y)
                    neighbors.add((x + 1, y))
                else:
                    distants.add((x + 1, y))

                if (((x, y) in sh) != ((x - 1, y) in sh)) and (x, y + 1) in rx:  # check border (x, y)::(x, y + 1)
                    neighbors.add((x, y + 1))
                else:
                    distants.add((x, y + 1))

                if (((x - 1, y) in sh) != ((x - 1, y - 1) in sh)) and (
                        x - 1, y) in rx:  # check border (x, y)::(x - 1, y)
                    neighbors.add((x - 1, y))
                else:
                    distants.add((x - 1, y))

                if (((x, y - 1) in sh) != ((x - 1, y - 1) in sh)) and (
                        x, y - 1) in rx:  # check border (x, y)::(x, y - 1)
                    neighbors.add((x, y - 1))
                else:
                    distants.add((x, y - 1))

                if not neighbors:
                    raise Exception(f"Vertex not found:\n{ls=}\n{rx=}\n{rc=}\n{r=}")

                neighbors.discard(lls)  # always deny the last vertex

                if len(neighbors) >= 3:
                    rx.add((x, y))  # we will reexplore this vertex later

                if len(neighbors) == 3:
                    ox, oy = lls or distants.pop()  # origin
                    vx = ox - x
                    vy = oy - y

                    lx = -vy  # leftward vector
                    ly = vx

                    nx = x + lx
                    ny = y + ly

                    cx = min(ox, nx)
                    cy = min(oy, ny)

                    if (cx, cy) in sh:
                        ls = nx, ny
                    else:
                        ls = x - lx, y - ly
                else:
                    for ls in neighbors:  # try to take a not explored vertex
                        if ls not in rc:
                            break

                lls = x, y
                rx.remove(ls)
                rc.append(ls)

            check = abs(rc[0][0] - rc[-1][0]) + abs(rc[0][1] - rc[-1][1])
            if check == 2:  # seams to occur only 1 time due to non-unique vertex reordering possibility
                ax, ay = rc[0]
                bx, by = rc[-1]
                c = (ax, by)
                d = (bx, ay)
                if rc[-2] == c or rc[1] == c: rc.append(d)
                elif rc[-2] == d or rc[1] == d: rc.append(c)
                else:
                    raise Exception(f"Failed to trivially fix malformed ring:\n{rc=}")
            elif check != 1:
                raise Exception(f"Non-trivially fixable malformed ring:\n{rc=}")

            rc.append(rc[0])  # close the ring
            rcs.append(rc)

        rings.append(rcs)
    return rings


def correcting_rings(rings: list[ShapeRings]) -> None:
    """
    Correct the rings orientation.

    Args:
        rings: the list of rings

    Notes:
        - outer rings are clockwise
        - inner rings are counterclockwise
    """

    for r in tqdm(rings, desc="Correcting rings\t", file=stdout, ncols=120, unit=" cloud"):
        policy = 1
        for rc in r:
            # check if not clockwise
            if policy * sum((rc[n + 1][0] - rc[n][0]) * (rc[n + 1][1] + rc[n][1]) for n in range(len(rc) - 1)) < 0:
                rc.reverse()
            policy = -1  # change the reverse policy for inner rings


def linking_rings(rings: list[ShapeRings]) -> list[Line]:
    """
    Link the rings into lines.

    Args:
        rings: the list of rings

    Returns: a list of lines, each line is a list of vertices

    Notes:
        - used to coonect the inner rings to the outer ring
        - inners and outers are required to be ordered and correctly oriented
        - only work with path following a direct grid (no diagonal) including all steps (even aligned vertices)
    """

    lines = []

    for r in tqdm(rings, desc="Linking rings\t\t", file=stdout, ncols=120, unit=" cloud"):
        r = r.copy()
        line = r.pop(0)  # outer ring
        while r:
            inner = r.pop()

            for cn, (cx, cy) in enumerate(inner):  # find one of the rightmost vertex
                if not any(True for vx, vy in inner if vy == cy and vx > cx):
                    break
            else:  # no rightmost vertex found
                raise Exception(f"Rightmost vertex not found:\n{inner=}")  # impossible

            hole_link = None
            dist = float("inf")

            for cn, (cx, cy) in enumerate(inner):
                for n, rg in enumerate([line] + r):  # check right-raycasting (cx, cy)
                    for u, (x, y) in enumerate(rg):
                        if (x - cx) ** 2 + (y - cy) ** 2 < dist:
                            dist = (x - cx) ** 2 + (y - cy) ** 2
                            hole_link = (cn, n, u)

            cn, ridx, idx = hole_link
            rx = line if not ridx else r.pop(ridx - 1)
            assert idx < len(rx) - 1
            rx = rx[:idx + 1] + inner[cn:] + inner[1:cn + 1] + rx[idx:]

            if not ridx:
                line = rx
            else:
                r.append(rx)

        lines.append(line)
    return lines


def simplify_lines(lines: list[Line], close: bool = False) -> None:
    """
    Simplify the lines by removing useless align vertices.

    Args:
        lines: the list of lines
        close: if the lines are closed
    """

    for n, ln in tqdm(enumerate(lines), desc="Simplifying lines\t", total=len(lines), file=stdout, ncols=120,
                      unit=" cloud"):
        lines[n] = simplify_line(ln, close)


def simplify_line(line: Line, close: bool = False) -> Line:
    """
    Simplify a line by removing useless align vertices.

    Args:
        line: the line to simplify
        close: if the line is closed

    Returns: the simplified line

    Notes:
        - the line must be opened (line[0] != line[-1])
        - must be at least 3 vertices
    """

    rn = []

    px, py = line[-2]
    cx, cy = line[-1]

    for nx, ny in line:
        vnx = nx - cx
        vny = ny - cy
        vpx = px - cx
        vpy = py - cy

        if vnx * vpy - vny * vpx != 0:  # is not align
            rn.append((cx, cy))
            px, py = cx, cy

        cx, cy = nx, ny

    if close:
        rn.append(rn[0])

    return rn


def isAlign(a: Vertex, b: Vertex, c: Vertex) -> bool:
    """
    Check if 3 vertices are align.

    Args:
        a: the first vertex
        b: the second vertex
        c: the third vertex

    Returns: True if the 3 vertices are align, False otherwise
    """

    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]) == 0


def ear_clipping(vertices):
    triangles = []
    V = list(vertices)  # Make a copy of the vertices list
    assert V[0] == V[-1]  # Ensure the polygon is closed
    V.pop()  # open
    R = []
    strict = True

    DEBUG = (2, 213) in V
    ITER = 0

    while len(V) > 3:
        for i in range(len(V)):
            prev = V[(i - 1) % len(V)]
            curr = V[i]
            next = V[(i + 1) % len(V)]

            if isConvex(prev, curr, next):
                S = []
                ear = True

                for j in range(len(V)):
                    if j not in [(i - 1) % len(V), i, (i + 1) % len(V)]:
                        inside_or_border, not_border = isPointInTriangle(V[j], prev, curr, next)
                        if strict:
                            if inside_or_border and V[j] not in [prev, curr, next]:
                                ear = False
                                break
                        else:
                            if inside_or_border:
                                jprev = V[(j - 1) % len(V)]
                                jcurr = V[j]
                                jnext = V[(j + 1) % len(V)]

                                cross = [isSegmentCrossing(jcurr, jprev, prev, next),
                                         isSegmentCrossing(jcurr, jprev, curr, next),
                                         isSegmentCrossing(jcurr, jprev, curr, prev),
                                         isSegmentCrossing(jcurr, jnext, prev, next),
                                         isSegmentCrossing(jcurr, jnext, curr, next),
                                         isSegmentCrossing(jcurr, jnext, curr, prev),
                                         isSegmentCrossing(jprev, jnext, prev, next),
                                         isSegmentCrossing(jprev, jnext, curr, next),
                                         isSegmentCrossing(jprev, jnext, curr, prev)]

                                if not_border or any(cross):
                                    ear = False
                                    break

                                elif V[j] not in [prev, curr, next]:
                                    S.append(V[j])

                if ear:
                    triangles.append((prev, curr, next))
                    R.append(V[i])
                    S.reverse()
                    V[i:i + 1] = []  # S
                    V = simplify_line(V)
                    break
        else:
            if strict:
                strict = False
                continue
            raise  # if reached, error in the polygon
        strict = True

    # Add the last remaining triangle
    if V:
        triangles.append((V[0], V[1], V[2]))
    return triangles


def plot_colored(vertices):
    n = len(vertices)
    colors = plt.cm.rainbow(np.linspace(0, 1, n))

    for i in range(n):
        x1, y1 = vertices[i]
        x2, y2 = vertices[(i + 1) % n]
        plt.plot([x1, x2], [y1, y2], color=colors[i])

    # plt.scatter(*zip(*vertices), color='black', s=1)  # Plot vertices


def isConvex(a, b, c):
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]) > 0


def isPointInTriangle(p, a, b, c):
    def sign(p1, p2, p3):
        return (p1[0] - p3[0]) * (p2[1] - p3[1]) - (p2[0] - p3[0]) * (p1[1] - p3[1])

    d1 = sign(p, a, b)
    d2 = sign(p, b, c)
    d3 = sign(p, c, a)
    has_neg = (d1 < 0) or (d2 < 0) or (d3 < 0)
    has_pos = (d1 > 0) or (d2 > 0) or (d3 > 0)
    return not (has_neg and has_pos), d1 != 0 and d2 != 0 and d3 != 0


def isSegmentCrossing(a, b, c, d):
    ab = (a[0] - b[0], a[1] - b[1])
    cd = (c[0] - d[0], c[1] - d[1])

    if (ab[0] * cd[1] - ab[1] * cd[0]) == 0:  # check if segments are parallel
        return False

    X = c[0] - a[0]
    A = c[1] - a[1]

    Y = d[0] - c[0]
    B = d[1] - c[1]

    Z = b[1] - a[1]
    C = b[0] - a[0]

    # keep k=kN/kD and t=tN/tD separately to avoid floating point precision issues
    kN = (C * A - Z * X)
    kD = (Y * Z - B * C)

    tN = X * kD + kN * Y
    tD = kD * C

    assert tN * kD * Z == tD * (A * kD + kN * B)

    # check 0 < kN/kD < 1 and 0 < tN/tD < 1
    if kD * kN <= 0: return False  # check k > 0
    if tD * tN <= 0: return False  # check t > 0

    if kN * kD >= kD * kD: return False  # check k < 1
    if tN * tD >= tD * tD: return False  # check t < 1

    return True


import matplotlib.patches as patches


def plot_triangulation(vertices, triangles, color="gray", pcolor=None, before=False, centers=True):
    if vertices:
        vertices.append(vertices[0])

    if before:
        plt.plot(*zip(*vertices), linewidth=2.5)

    for triangle in triangles:
        t = list(triangle)
        t.append(t[0])
        plt.plot(*zip(*t), color=color)
        x, y = zip(*triangle)
        if centers:
            plt.scatter(sum(x) / 3, sum(y) / 3, s=.6, color=(.8, .6, .2))

    if not before:
        plt.plot(*zip(*vertices))

    if vertices:
        vertices.pop(-1)

    if pcolor:
        plt.scatter(*zip(*vertices), s=.6, color=pcolor)


def triangulate_lines(lines: list[Line]) -> list[Polygon]:
    """
    Triangulate the lines.

    Args:
        lines: the list of lines

    Returns: a list of polygons, each polygon is a list of triangles
    """

    polygons = []

    for ln in tqdm(lines, desc="Triangulating\t\t", file=stdout, ncols=120, unit=" cloud"):
        polygons.append(ear_clipping(list(reversed(ln))))

    return polygons


def optimize_polygons(polygons: list[Polygon]) -> list[Polygon]:
    """
    Optimize the polygons by removing the useless triangles.

    Args:
        polygons: the list of polygons

    Returns: the optimized polygons
    """

    sp = so = 0

    for n, p in tqdm(enumerate(polygons), desc="Optimizing\t\t\t", total=len(polygons), file=stdout, ncols=120,
                     unit=" cloud"):
        pg = PolygonGraph(p)
        pg.optimize()

        op = pg.polygon

        sp += len(p)
        so += len(op)

        polygons[n] = op

    print(f"Optimization: {sp} vtx to {so} vtx")
    return polygons


class LinkedTriangle:
    def __init__(self, t: Triangle):
        self.vtx = frozenset(t)
        self.edges = frozenset[Edge](frozenset((t[v - 1], t[v])) for v in range(3))
        self.neighbors = dict[Edge, tuple[LinkedTriangle, Vertex, Vertex]]()

    def __iter__(self) -> Iterator[Edge]:
        return iter(self.edges)

    def link(self, edge: Edge, neighbor: Self) -> None:
        self.neighbors[edge] = neighbor, set(self.vtx - neighbor.vtx).pop(), set(neighbor.vtx - self.vtx).pop()

    def unlink(self):

        for edge, (neighbor, own, ext) in self.neighbors.items():
            neighbor.neighbors.pop(edge)

        self.neighbors.clear()

    def swap(self, reverse: bool = False) -> None | tuple[set[LinkedTriangle], set[LinkedTriangle]]:

        for (x, y), (neighbor, own, ext) in self.neighbors.items():

            if self.circle_contains(ext, reverse) and (not reverse or (
                    isConvex(own, x, y) == isConvex(own, x, ext) and isConvex(ext, x, y) == isConvex(ext, own, y)
            )  # check in reverse mode that the swap will not create concave overlapping triangles
            ):
                return {LinkedTriangle((own, ext, x)), LinkedTriangle((own, ext, y))}, {self, neighbor}

    def merge(self) -> None | tuple[set[LinkedTriangle], set[LinkedTriangle]]:
        return self.merge_edge_all() or self.merge_edge_one() or self.merge_edge_quad()

    def merge_edge_one(self) -> None | tuple[set[LinkedTriangle], set[LinkedTriangle]]:

        for (x, y), (neighbor, own, ext) in self.neighbors.items():

            if isAlign(own, ext, x):
                return {LinkedTriangle((own, ext, y))}, {self, neighbor}

            elif isAlign(own, ext, y):
                return {LinkedTriangle((own, ext, x))}, {self, neighbor}

    def merge_edge_quad(self) -> None | tuple[set[LinkedTriangle], set[LinkedTriangle]]:
        neighbors = self.neighbors.copy()

        while len(neighbors) >= 2:
            edge1, (neighbor1, own1, ext1) = neighbors.popitem()

            for edge2, (neighbor2, own2, ext2) in neighbors.items():

                if isAlign(ext1, ext2, inter := set(edge1 & edge2).pop()):

                    if isConvex(own1, own2, inter) == isConvex(own1, own2, ext1):
                        new_edge1, new_edge2 = own1, ext1
                    else:
                        new_edge1, new_edge2 = own2, ext2

                    return ({
                                LinkedTriangle((ext1, ext2, new_edge1)),
                                LinkedTriangle((own1, own2, new_edge2))
                            },
                            {self, neighbor1, neighbor2})

    def merge_edge_all(self) -> None | tuple[set[LinkedTriangle], set[LinkedTriangle]]:

        if len(self.neighbors) != 3:
            return None

        (edge1, (neighbor1, own1, ext1)), (edge2, (neighbor2, own2, ext2)), (
            edge3, (neighbor3, own3, ext3)) = self.neighbors.items()

        if isAlign(ext1, ext2, own3) and isAlign(ext2, ext3, own1) and isAlign(ext3, ext1, own2):
            return {LinkedTriangle((ext1, ext2, ext3))}, {self, neighbor1, neighbor2, neighbor3}

    @property
    def triangle(self) -> Triangle:
        t = tuple(self.vtx)
        if not isConvex(*t):
            t = t[::-1]

        return t

    def _circle_contains(self, vtx: Vertex) -> int:
        (ax, ay), (bx, by), (cx, cy) = self.triangle
        dx, dy = vtx

        # inline the determinant calculation to keep int full precision
        a, b = ax - dx, ay - dy
        d, e = bx - dx, by - dy
        g, h = cx - dx, cy - dy
        c, f, i = a ** 2 + b ** 2, d ** 2 + e ** 2, g ** 2 + h ** 2

        return a * e * i + b * f * g + c * d * h - c * e * g - b * d * i - a * f * h

    def circle_contains(self, vtx: Vertex, reverse: bool = False) -> bool:
        if reverse:
            return self._circle_contains(vtx) < 0
        return self._circle_contains(vtx) > 0

    def __repr__(self):
        return f"LT<{tuple(self.vtx)}>"

    def __hash__(self):
        return hash(self.vtx)


class PolygonGraph:
    """
    Build the linked triangles graph from a triangles list for easy graph algo impl.
    """

    def __init__(self, polygon: Polygon) -> None:
        # prepare the graph structure
        self.tri: dict[Edge, set[LinkedTriangle]] = {}  # edge -> list of LinkedTriangle that use this edge (max 2)
        self.all_tri = set[LinkedTriangle]()

        self.DEBUG = False
        # self.DEBUG = any(i for i in polygon if (0, 42) in i)

        # setup the data structures: linked triangles indexed by edges
        for tr in polygon:
            self.all_tri.add(lt := LinkedTriangle(tr))

            for edge in lt:
                self.tri.setdefault(edge, set()).add(lt)

        # link the triangles by edges
        for edge, lts in self.tri.items():
            if len(lts) == 2:  # len(lts) == 1 -> border, 2 -> internal
                x, y = lts

                x.link(edge, y)
                y.link(edge, x)

    def apply(self, news: set[LinkedTriangle], olds: set[LinkedTriangle]) -> set[LinkedTriangle]:

        new_edges = set[Edge]()

        self.all_tri -= olds
        self.all_tri |= news

        for old in olds:  # remove the old triangles
            old.unlink()

            for edge in old:  # update the linked triangles
                lk = self.tri[edge]
                lk.remove(old)

                if not lk:
                    self.tri.pop(edge)

        for new in news:  # add the new triangles
            for edge in new:  # update the linked triangles
                new_edges.add(edge)
                self.tri.setdefault(edge, set()).add(new)

        # link the triangles by edges
        for edge in new_edges:
            lts = self.tri[edge]
            news |= lts  # 'news' will contains all affected triangles (new onces and their neighbors)

            if len(lts) == 2:  # len(lts) == 1 -> border, 2 -> internal
                x, y = lts

                x.link(edge, y)
                y.link(edge, x)

        return news

    def optimize(self) -> None:

        d = self.DEBUG
        self.DEBUG = False
        self.optimize_geometry()

        self.DEBUG = d
        self.optimize_delaunay()

        while True:  # destroying and rebuilding delaunay alternately until no more changes
            self.optimize_delaunay(True)

            last = len(self.all_tri)
            self.optimize_delaunay()

            if last == len(self.all_tri):
                break

        #for i in self.all_tri:
        #    n = set(i.vtx | {n for u, _, _ in i.neighbors.values() for n in u.vtx})
        #    q = 6
        #    while n:
        #        x = n.pop()
        #        for a in n:
        #            for b in n:
        #                if a is b: continue
        #                if isAlign(x, a, b):
        #                    q -= 1
        #    assert q == 6

    def optimize_geometry(self, base: set[LinkedTriangle] = None) -> None:

        if not base:
            base = self.all_tri

        while True:

            for lt in base:

                if r := lt.merge():  # if merged successfully
                    base.update(self.apply(*r))

                    if self.DEBUG:
                        self.plot(f"Geometry [{len(base)}]")

                    break
            else:
                break

    def optimize_delaunay(self, reverse: bool = False) -> None:
        cx = []
        while True:

            for lt in self.all_tri:

                if r := lt.swap(reverse):  # if swapped successfully
                    cx.append(lt)
                    self.optimize_geometry(self.apply(*r))

                    if self.DEBUG:
                        self.plot(f"Delaunay [{len(self.all_tri)}]")

                    break
            else:
                break

    @property
    def polygon(self) -> Polygon:
        return [i.triangle for i in self.all_tri]

    def plot(self, title: str = "") -> None:
        plt.title(title)
        plot_triangulation([], self.polygon)
        plt.show()


def triangle_center(triangle: Triangle) -> Vertex:
    x1, y1 = triangle[0]
    x2, y2 = triangle[1]
    x3, y3 = triangle[2]
    return floor((x1 + x2 + x3) / 3), floor((y1 + y2 + y3) / 3)


def pack_triangles(polygons: list[Polygon], lines: list[Line], size: int, region: int = 8, thickness: int = 1) -> list[
    list[Polygon3D]]:
    """
    Pack the triangles by local regions

    Args:
        polygons: the list of polygons
        lines: the list of lines
        size: the size of the cloud space (texture size)
        region: the region size (size of grid cells)
        thickness: the thickness of the border quads

    Returns: the packed polygon
    """

    grid = [[[] for _ in range(0, size, region)] for _ in range(0, size, region)]
    mr = (size - 1) // region

    for pl in tqdm(polygons, desc="Packing\t\t\t\t", file=stdout, ncols=120, unit=" cloud"):

        for tr in pl:
            x, y = triangle_center(tr)
            (x1, y1), (x2, y2), (x3, y3) = tr
            assert isConvex((x1, y1), (x2, y2), (x3, y3))
            rx, ry = x // region, y // region
            sx, sy = rx * region, ry * region
            grid[rx][ry].append(((x1 - sx, 0, y1 - sy), (x2 - sx, 0, y2 - sy), (x3 - sx, 0, y3 - sy)))

    for ln in lines:
        llast = ln[-2]
        last = ln[-1]

        for vtx in ln:
            x1, y1 = last
            x2, y2 = vtx

            x, y = floor((x1 + x2) / 2), floor((y1 + y2) / 2)

            rx, ry = min(x // region, mr), min(y // region, mr)
            sx, sy = rx * region, ry * region

            tr1 = ((x1 - sx, 0, y1 - sy), (x2 - sx, 0, y2 - sy), (x2 - sx, thickness, y2 - sy))
            tr2 = ((x1 - sx, 0, y1 - sy), (x2 - sx, thickness, y2 - sy), (x1 - sx, thickness, y1 - sy))

            if isConvex(llast, last, vtx):
                if isConvex(*tr1):
                    pass
                else:
                    pass
            else:
                pass

            cell = grid[rx][ry]
            cell.append(tr1)
            cell.append(tr2)

            llast = last
            last = vtx

    print()
    print("Lowest  region size:", min(len(i) for j in grid for i in j if i))
    print("Highest region size:", max(len(i) for j in grid for i in j))
    print("Average region size:", sum(len(i) for j in grid for i in j if i) / (mr * mr))

    return grid


def export_grid(grid: list[list[Polygon3D]], header: str, source: str) -> None:
    """
    Export the grid to a file.

    Args:
        grid: the grid
        header: the C header file to write
        source: the C source file to write
    """

    payload_inc = """
// DO NOT EDIT: AUTOGENERATED FILE (by assets/gennerators/textures.cloud.py, called from CMakeLists.txt)
#pragma once

#include <gccore.h>

"""

    payload_src = f"""
// DO NOT EDIT: AUTOGENERATED FILE (by assets/gennerators/textures.cloud.py, called from CMakeLists.txt)
#include "assets/cloud.h"
#include "engine/render/vertex.h"
#include "engine/env/light.h"

"""

    vtx_type = "ColoredVertex"
    exists = set()

    n = 0

    print(f"Exporting cloud regions to {header} and {source}...")

    for n, row in enumerate(grid):

        for m, cell in enumerate(row):

            if cell:
                exists.add((n, m))
                payload_src += f"const DisplayList<{3 * len(cell)}, {vtx_type}> CloudRegion_{n}_{m} = {{.vertices = {{\n"

                for tr in cell:

                    is_x = all(vtx[0] == tr[0][0] for vtx in tr)
                    is_wall = any(vtx for vtx in tr if vtx[1])

                    for vtx in tr:
                        # is_wall = vtx[1] > 0  # cause funny gradient
                        color = f"CloudBorder{'X' if is_x else 'Z'}" if is_wall else "Cloud"
                        payload_src += f"\t\t{{ {vtx[0]}, {vtx[1]}, {vtx[2]}, static_cast<u16>(LightColor::{color}) }},\n"

                payload_src += "\t}\n};\n\n"

    n += 1
    payload_inc += f"constexpr u32 CLOUD_REGION_SIZE = {n};\n\n"
    payload_inc += f"extern const void * CloudRegions[{n}][{n}];\n"
    payload_src += f"const void * CloudRegions[{n}][{n}] = {{\n"

    for x in range(n):
        payload_src += "\t{"

        for y in range(n):

            if (x, y) in exists:
                payload_src += f"static_cast<const void *>(&CloudRegion_{x}_{y}), "
            else:
                payload_src += "nullptr, "

        payload_src += "},\n"

    payload_src += "};\n"

    with open(header, "w") as f:
        f.write(payload_inc)

    with open(source, "w") as f:
        f.write(payload_src)

    print(f"Done!")


def generate(cloud_tex: str, src: str, inc: str, debug: bool = False) -> None:
    """
    Generate the cloud geometry from the cloud image.

    Args:
        cloud_tex: the cloud image path
        src: the source file path
        inc: the header file path
        debug: if debug mode is enabled
    """

    print(f"Loading {cloud_tex}...")
    cloud = cv2.imread(cloud_tex, cv2.IMREAD_UNCHANGED)
    cloud = np.transpose(cloud, (1, 0, 2))
    cloud = np.apply_along_axis(lambda x: x[3] > 128, 2, cloud)
    size, height = cloud.shape
    assert size == height

    shapes = extract_shapes(cloud)
    borders = extract_borders(shapes)
    vertices = extract_vertices(shapes, borders)
    rings = split_vertices(vertices, shapes)
    correcting_rings(rings)
    lines = linking_rings(rings)
    border_lines = [i for r in rings for i in r]
    simplify_lines(lines, close=True)
    simplify_lines(border_lines, close=True)
    polygons = triangulate_lines(lines)
    optimize_polygons(polygons)
    grid = pack_triangles(polygons, border_lines, size)
    export_grid(grid, inc, src)

    if debug:
        print("Debug plotting...")

        plt.figure(figsize=(12, 12))
        ax = plt.gca()
        ax.invert_yaxis()
        ax.set_aspect("equal")
        plt.title("Shapes")
        plt.tight_layout()

        for sh in tqdm(shapes, desc="Plotting shapes\t\t", file=stdout, ncols=120, unit=" cloud"):
            plt.scatter(*zip(*sh), s=1)

        plt.figure(figsize=(12, 12))
        ax = plt.gca()
        ax.invert_yaxis()
        ax.set_aspect("equal")
        plt.title("Vertices")
        plt.tight_layout()

        for vtx in tqdm(vertices, desc="Plotting vertices\t", file=stdout, ncols=120, unit=" cloud"):
            plt.scatter(*zip(*vtx), s=1, marker='s')

        plt.figure(figsize=(12, 12))
        ax = plt.gca()
        ax.invert_yaxis()
        ax.set_aspect("equal")
        plt.title("Rings")
        plt.tight_layout()

        for r in tqdm(rings, desc="Plotting rings\t\t", file=stdout, ncols=120, unit=" cloud"):
            for n, rc in enumerate(r):
                plot_colored(rc)

        plt.figure(figsize=(12, 12))
        ax = plt.gca()
        ax.invert_yaxis()
        ax.set_aspect("equal")
        plt.title("Lines")
        plt.tight_layout()

        for r in tqdm(lines, desc="Plotting lines\t\t", file=stdout, ncols=120, unit=" cloud"):
            plt.plot(*zip(*r))

        plt.figure(figsize=(12, 12))
        ax = plt.gca()
        ax.invert_yaxis()
        ax.set_aspect("equal")
        plt.title("Triangles")
        plt.tight_layout()

        for n, (tr, ln) in tqdm(enumerate(zip(polygons, lines)), desc="Plotting triangles\t", total=len(polygons), file=stdout, ncols=120, unit=" cloud"):
            plot_triangulation(ln, tr, before=True, centers=False)

        print("Plotting...")
        plt.show()


if __name__ == "__main__":
    debug = False

    if debug:
        import matplotlib.pyplot as plt

        sys.argv = ("", "../textures/cloud.png", "test.c", "test.h")

    generate(*sys.argv[1:], debug=debug)
