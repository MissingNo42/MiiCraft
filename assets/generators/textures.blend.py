"""
Blends the images in the given list and saves the result to the output file.
"""

import sys
import cv2


def generate(files: tuple[str], output: str):
    """
    Blends the images in the given list and saves the result to the output file.

    Args:
        files: the files to process
        output: the output texture
    """

    dst = None

    for file in files:
        src = cv2.imread(file, cv2.IMREAD_UNCHANGED)

        if dst is not None:
            dst += src
        else:
            dst = src

    cv2.imwrite(output, dst, [cv2.IMWRITE_PNG_COMPRESSION, 0])


if __name__ == '__main__':
    _, *files, output = sys.argv
    generate(files, output)