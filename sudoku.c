#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "mem.h"
#include "pnmrdr.h"
#include "uarray2.h"

#define N 9

/* bit mask helper: bit v lives at (1 << v), with v in 1..9 */
static inline int add_or_dup(int mask, int v)
{
    assert(1 <= v && v <= 9);
    int bit = 1 << v;
    if (mask & bit) return -1;  
    return mask | bit;
}

static int check_rows(UArray2_T g)
{
    for (int j = 0; j < N; j++) {
        int mask = 0;
        for (int i = 0; i < N; i++) {
            int v = *(int *)UArray2_at(g, i, j);
            if (v < 1 || v > 9) return 0;
            mask = add_or_dup(mask, v);
            if (mask < 0) return 0;
        }
    }
    return 1;
}

static int check_cols(UArray2_T g)
{
    for (int i = 0; i < N; i++) {
        int mask = 0;
        for (int j = 0; j < N; j++) {
            int v = *(int *)UArray2_at(g, i, j);
            if (v < 1 || v > 9) return 0;
            mask = add_or_dup(mask, v);
            if (mask < 0) return 0;
        }
    }
    return 1;
}

static int check_blocks(UArray2_T g)
{
    for (int bj = 0; bj < 3; bj++) {
        for (int bi = 0; bi < 3; bi++) {
            int mask = 0;
            for (int dj = 0; dj < 3; dj++) {
                for (int di = 0; di < 3; di++) {
                    int i = bi * 3 + di;
                    int j = bj * 3 + dj;
                    int v = *(int *)UArray2_at(g, i, j);
                    if (v < 1 || v > 9) return 0;
                    mask = add_or_dup(mask, v);
                    if (mask < 0) return 0;
                }
            }
        }
    }
    return 1;
}

int main(int argc, char *argv[])
{
    /* args per spec: 0 args -> stdin; 1 arg -> filename; >1 -> CRE */
    FILE *fp = NULL;
    if (argc == 1) {
        fp = stdin;
    } else if (argc == 2) {
        fp = fopen(argv[1], "rb");
        assert(fp != NULL);              
    } else {
        assert(0 && "sudoku takes at most one argument");
    }

    /* Read header with Pnmrdr; assert it is a graymap with 9x9 dims and denom==9 */
    Pnmrdr_T rdr = Pnmrdr_new(fp);
    Pnmrdr_mapdata md = Pnmrdr_data(rdr);

    /* The interface defines a type field; assert it's graymap as required by spec. */
    assert(md.type == Pnmrdr_gray);     
    assert(md.width  == N && md.height == N);
    assert(md.denominator == 9);

    /* Build a 9x9 grid of ints and read all pixels */
    UArray2_T grid = UArray2_new(N, N, sizeof(int));
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            int val = Pnmrdr_get(rdr);  
            *(int *)UArray2_at(grid, i, j) = val;
        }
    }

    Pnmrdr_free(&rdr);
    if (fp != stdin) fclose(fp);

    /* Validate rows, columns, and 3x3 blocks */
    int ok = check_rows(grid) && check_cols(grid) && check_blocks(grid);

    UArray2_free(&grid);

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}