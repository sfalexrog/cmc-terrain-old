# Diamond-square procedural algorithm documentation

Diamond-square algorithm is one of the algorithms that can be used to generate procedural terrain.

It can be used to generate heightmaps of sizes `2**n + 1`.

There are two basic steps in the algorithm:

* diamond step: get height values for four points that are vertices of a square, average the values, add random value, set midpoint to the resulting height;
* square step: get heigth values for four points that are vertices of a diamond, average the values, add random value, set midpoint to the resulting height.

The algorithm starts with four corners of the heightmap initialized (possibly to random values). Next, the diamond step is performed for the corner points, which generates a midpoint. The square step is then performed, resulting in four midpoints on the edges. The algorithm continues until the whole heightmap is generated.

Note that some of the square steps will need out-of-bounds values. This can be handled by "wrapping around" the indices.

## Reusing edge values to generate perfect tiling

The resulting heightmap may be perfectly tiled if edge elements have the same value (as in, `h[0, y] == h[2**n, y]`, `h[x, 0] == h[x, 2**n]`, for `x, y` in `0..2**n`). Note that assigning edge values after generation is not enough, this has to be used during the generation step. This can be accomplished by using a `2**n` by `2**n` array, with a custom accessor that will give you elements modulo `2**n`.

## Perfect continuation

As a corollary to the previous point, an existing heightmap may be used to generate a new one, with its height values on the edge set to the values of the existing one.
