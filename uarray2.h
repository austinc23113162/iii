/**************************************************************
 *
 *                       uarray2.h
 *
 *     Assignment: iii (CS 40 A2)
 *     Authors:    tvales01, 
 *     Date:       <2025-09-25>
 *
 *     Public interface for a 2-D unboxed, polymorphic array. Clients
 *     supply element size. Provides width/height/size queries, pointer
 *     access at (col,row), and row/col-major mapping callbacks.
 *
 *     Indices and order:
 *       i = column (0..width-1), j = row (0..height-1).
 *       Row-major map: rows outer, columns inner.
 *       Col-major map: columns outer, rows inner.
 *
 *     Notes:
 *       UArray2_at returns a pointer to element storage valid until the
 *       array is freed. Function contracts are documented in uarray2.c.
 *
 **************************************************************/

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#define T UArray2_T
typedef struct T *T;


extern T UArray2_new(int col, int row, int size);

extern int UArray2_width(T uarray2);

extern int UArray2_height(T uarray2);

extern int UArray2_size(T uarray2);

extern void *UArray2_at(T uarray2, int col, int row);

extern void UArray2_map_col_major(T uarray2,
                                  void apply(int col, int row, T uarray2,
                                             void *p1, void *cl),
                                  void *cl);

extern void UArray2_map_row_major(T uarray2,
                                  void apply(int col, int row, T uarray2,
                                             void *p1, void *cl),
                                  void *cl);

extern void UArray2_free(T *uarray2);

#undef T
#endif