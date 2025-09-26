/**************************************************************
 *
 *                       bit2.h
 *
 *     Assignment: iii (CS 40 A2)
 *     Authors:    <tvales01>
 *     Date:       <2025-09-25>
 *
 *     Public interface for a 2-D array of bits (0/1). Provides
 *     width/height, get/put of a bit at (col,row), and row/col-major
 *     mapping that passes the bit value. No pointer-to-bit API.
 *
 *     Indices and order:
 *       i = column (0..width-1), j = row (0..height-1).
 *       Row-major map: rows outer, columns inner.
 *       Col-major map: columns outer, rows inner.
 *
 *     Notes:
 *       get returns 0 or 1; put sets 0/1 and returns previous value.
 *       Function contracts are documented in bit2.c.
 *
 **************************************************************/

#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED

typedef struct Bit2_T *Bit2_T;

extern Bit2_T Bit2_new(int col, int row);
extern void Bit2_free(Bit2_T *bit2);

extern int Bit2_width (Bit2_T bit2);
extern int Bit2_height(Bit2_T bit2);

extern int Bit2_get(Bit2_T bit2, int col, int row);
extern int Bit2_put(Bit2_T bit2, int col, int row, int bit);

extern void Bit2_map_row_major(
        Bit2_T bit2,
        void apply(int col, int row, Bit2_T bit2, int bit, void *cl),
        void *cl);

extern void Bit2_map_col_major(
        Bit2_T bit2,
        void apply(int col, int row, Bit2_T bit2, int bit, void *cl),
        void *cl);

#endif 