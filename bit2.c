/**************************************************************
 *
 *                       bit2.c
 *
 *     Assignment: iii (CS 40 A2)
 *     Authors:    <tvales01, >
 *     Date:       <2025-09-25>
 *
 *     Implementation of a 2-D bit grid using Hanson Bit_T per column.
 *     Representation is an array-of-columns: outer UArray_T(length
 *     = width) whose elements are Bit_T vectors of length height. Bits
 *     are packed by Bit_T; API is value-based (get/put), plus row/col
 *     mapping that passes the current bit value.
 *
 *     Dependencies:
 *       assert.h (Hanson), mem.h (NEW/FREE),
 *       uarray.h (outer array), bit.h (Bit_T).
 *
 *     Indices and order:
 *       i = column, j = row.
 *       Row-major: j outer, i inner.  Col-major: i outer, j inner.
 *
 *     Representation invariant:
 *       width >= 0; height >= 0.
 *       UArray_size(outer_bits) == sizeof(Bit_T).
 *       For each column i: Bit_length(inner) == height.
 *       Stored values are exactly {0,1}.
 *
 *     Checked runtime errors (CREs):
 *       Bit2_new: width<0 || height<0.
 *       Bit2_get/put: NULL handle, OOB indices; put: bit ∉ {0,1}.
 *       Maps: NULL handle or NULL apply.
 *       Bit2_free: NULL pointer or *ptr==NULL.
 *
 **************************************************************/

#include "bit2.h"
#include "bit.h"
#include "uarray.h"
#include "assert.h"
#include "mem.h"

#include <stdlib.h>

struct Bit2_T {
        int width;
        int height;
        UArray_T outer_bits;
};

/********** Bit2_new ********
 * Create a 2-D bit grid of size col×row with all bits initialized to 0.
 *
 * Parameters:
 *      int col:  number of columns (width), must be >= 0
 *      int row:  number of rows (height), must be >= 0
 *
 * Returns:
 *      Bit2_T: newly allocated bit grid
 *
 * Effects:
 *      Allocates heap memory for the Bit2 and its columns (Bit_T per column).
 *
 * Checked runtime errors (CRE):
 *      CRE if col < 0 or row < 0
 *      May CRE on allocation failure (via Hanson mem/bit)
 ************************/
Bit2_T Bit2_new(int col, int row)
{
        Bit2_T bit2;
        NEW(bit2);

        assert(col >= 0 && row >= 0);
        bit2->width = col;
        bit2->height = row;
        bit2->outer_bits = UArray_new(col, sizeof(Bit_T));

        for (int i = 0; i < col; i++)
        {
                Bit_T inner_bits = Bit_new(row);

                Bit_T *innerp = UArray_at(bit2->outer_bits, i);
                *innerp = inner_bits;
        }
        
        return bit2;
}

/********** Bit2_width / Bit2_height ********
 * Return the grid dimensions.
 *
 * Parameters:
 *      Bit2_T bit2: non-NULL grid
 *
 * Returns:
 *      int: width (columns) or height (rows)
 *
 * CRE
 *      CRE if bit2 == NULL
 ************************/
int Bit2_width(Bit2_T bit2)
{
        assert(bit2 != NULL);
        return bit2->width;
}

int Bit2_height(Bit2_T bit2)
{
        assert(bit2 != NULL);
        return bit2->height;
}

/********** Bit2_get ********
 * Read the bit at (col,row).
 *
 * Parameters:
 *      Bit2_T bit2: non-NULL grid
 *      int col:     0 <= col < width
 *      int row:     0 <= row < height
 *
 * Returns:
 *      int: 0 or 1 (the stored bit)
 *
 * CRE
 *      CRE if bit2 == NULL
 *      CRE if indices are out of bounds
 ************************/
int Bit2_get(Bit2_T bit2, int col, int row)
{
        assert(bit2 != NULL);
        assert(col >= 0 && col < bit2->width);
        assert(row >= 0 && row < bit2->height);

        Bit_T *innerp = UArray_at(bit2->outer_bits, col);
        Bit_T inner = *innerp;
        assert(sizeof(inner) == UArray_size(bit2->outer_bits));

        return Bit_get(inner, row);
}

/********** Bit2_put ********
 * Write the bit at (col,row), returning the previous value.
 *
 * Parameters:
 *      Bit2_T bit2: non-NULL grid
 *      int col:     0 <= col < width
 *      int row:     0 <= row < height
 *      int bit:     must be 0 or 1
 *
 * Returns:
 *      int: previous bit (0 or 1)
 *
 * CRE
 *      CRE if bit2 == NULL
 *      CRE if indices are out of bounds
 *      CRE if bit ∉ {0,1}
 ************************/
int Bit2_put(Bit2_T bit2, int col, int row, int bit)
{
        assert(bit2 != NULL);
        assert(col >= 0 && col < bit2->width);
        assert(row >= 0 && row < bit2->height);
        assert(bit == 0 || bit == 1);

        Bit_T *innerp = UArray_at(bit2->outer_bits, col);
        Bit_T inner = *innerp;
        assert(sizeof(inner) == UArray_size(bit2->outer_bits));

        int prev = Bit_get(inner, row);

        Bit_put(inner, row, bit);
        return prev;
}

 /********** Bit2_map_row_major ********
 * Visit every element in row-major order and call apply for each.
 *
 * Parameters:
 *      Bit2_T bit2: grid
 *      void apply(int col, int row, Bit2_T b, int value, void *cl):
 *                   client callback; value is the bit at (col,row) at call time
 *      void *cl:    closure pointer passed to each call
 *
 * Returns:
 *      None
 *
 * Order:
 *      Rows outermost (row = 0..height-1), columns innermost (col = 0..width-1).
 *
 * CRE
 *      CRE if bit2 == NULL or apply == NULL
 ************************/
void Bit2_map_row_major(Bit2_T bit2,
                                  void apply(int col, int row, Bit2_T bit2,
                                             int bit, void *cl),
                                  void *cl)
{
        assert(bit2 != NULL);
        UArray_T outer_bits = bit2->outer_bits;

        for (int row = 0; row < bit2->height; row++) {
                for (int col = 0; col < bit2->width; col++) {
                        Bit_T *innerp = UArray_at(outer_bits, col);
                        Bit_T inner = *innerp;
                        assert(sizeof(inner) == UArray_size(outer_bits));

                        apply(col, row, bit2, Bit_get(inner, row), cl);
                }
        }
}

/********** Bit2_map_col_major ********
 * Same as above but column-major: columns outermost, rows innermost.
 ************************/
void Bit2_map_col_major(Bit2_T bit2,
                                  void apply(int col, int row, Bit2_T bit2,
                                             int bit, void *cl),
                                  void *cl)
{
        assert(bit2 != NULL);
        UArray_T outer_bits = bit2->outer_bits;

        for (int col = 0; col < bit2->width; col++) {
                Bit_T *innerp = UArray_at(outer_bits, col);
                Bit_T inner = *innerp;
                assert(sizeof(inner) == UArray_size(outer_bits));

                for (int row = 0; row < bit2->height; row++) {
                        apply(col, row, bit2, Bit_get(inner, row), cl);
                }
        }

}

/********** Bit2_free ********
 * Dispose of a Bit2 grid and set *bit2 to NULL.
 *
 * Parameters:
 *      Bit2_T *bit2: pointer to a Bit2 handle; *bit2 must be non-NULL
 *
 * Returns:
 *      None
 *
 * Effects:
 *      Frees all columns and the outer structure; sets *bit2 = NULL.
 *
 * CRE
 *      CRE if bit2 == NULL or *bit2 == NULL
 ************************/
void Bit2_free(Bit2_T *bit2) {
        assert(bit2 != NULL && *bit2 != NULL);

        for (int col = 0; col < (*bit2)->width; col++) {
                Bit_T *innerp = UArray_at((*bit2)->outer_bits, col);
                Bit_free(innerp);
        }
        UArray_free(&(*bit2)->outer_bits);
        FREE(*bit2);
}