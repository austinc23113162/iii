/**************************************************************
 *
 *                       sudoku.c
 *
 *     Assignment: iii (CS 40 A2)
 *     Authors:    <tvales01, >
 *     Date:       <2025-09-25>
 *
 *     Predicate program: exit(0) iff input is a solved 9×9 Sudoku.
 *     Reads a PGM (graymap) from stdin or one filename with Pnmrdr;
 *     asserts: type==gray, width==height==9, denominator==9. Loads a
 *     9×9 UArray2<int>, then verifies each row, column, and each 3×3
 *     block contains digits 1..9 exactly once. Prints nothing; exit
 *     status is the only result (0=solved, 1=not solved).
 *
 *     Dependencies:
 *       pnmrdr.h, uarray2.h, assert.h, mem.h, stdlib/stdio.
 *
 *     Checked runtime errors (CREs):
 *       >1 argument; file open failure; not a graymap; wrong dims or
 *       denominator; malformed data as detected by Pnmrdr.
 *
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "mem.h"
#include "pnmrdr.h"
#include "uarray2.h"

#define N 9

/********** add_or_dup (static helper) ********
 * Add digit v (1..9) to a bitmask of seen digits, or report duplicate.
 *
 * Parameters:
 *      int mask: current mask of seen digits (bits 1..9)
 *      int v:    digit in 1..9
 *
 * Returns:
 *      int: updated mask, or -1 if v was already present
 *
 * CRE
 *      CRE if v ∉ [1,9]
 ************************/
static inline int add_or_dup(int mask, int v)
{
    assert(1 <= v && v <= 9);
    int bit = 1 << v;
    if (mask & bit) return -1;  
    return mask | bit;
}

/********** check_rows / check_cols / check_blocks (static) ********
 * Validate that each row/column/3×3 block contains digits 1..9 exactly once.
 *
 * Parameters:
 *      UArray2_T g: 9×9 array of ints
 *
 * Returns:
 *      int: 1 if valid, 0 if any violation (out of range or duplicate)
 *
 * Expects:
 *      g not NULL; size of each element is sizeof(int).
 ************************/
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

/********** main ********
 * Predicate program: exit 0 iff input is a solved 9×9 Sudoku PGM.
 *
 * Parameters:
 *      argc/argv: 0 args -> read stdin; 1 arg -> open filename
 *
 * Behavior:
 *      Uses Pnmrdr to read a graymap; asserts md.type == Pnmrdr_gray,
 *      md.width == md.height == 9, and md.denominator == 9.
 *      Reads 81 values into a 9×9 UArray2<int>; validates rows/cols/blocks.
 *
 * Returns:
 *      EXIT_SUCCESS (0) if solved; EXIT_FAILURE (1) otherwise.
 *
 * CRE
 *      CRE if >1 argument
 *      CRE if file open fails
 *      CRE if image is not a graymap or has wrong dimensions/denominator
 ************************/
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