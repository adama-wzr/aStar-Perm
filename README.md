# aStar-Perm
a* algorithm for detecting if there possible paths between the left and right side of a given domain.

To compile simply use g++:

g++ aStar.cpp -lm

The code presented here is a modification from this algorithm: https://www.geeksforgeeks.org/a-search-algorithm/

The key differences are:

- The paths are traced from the starting point (entire left boundary) to the end point (entire right boundary).
- Diagonals are not considered, only the 4 neighbors of each cell.
- Periodic boundary conditions at the top and bottom.
- It reads a grayscale .jpg image and prints another grayscale .jpg image if a path was found.
