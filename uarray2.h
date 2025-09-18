#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#define T UArray2_T
typedef struct T *T;

/********** UArray2_new ********
 *
 * Allocates, initializes, and returns a new 2d array of width col and length
 * row where each element occupies size bytes
 *
 * Parameters:
 *      int col: the number of columns of the 2d array
 *      int row: the number of rows in the 2d array
 *      int size: the number of bytes each element will occupy
 *
 *
 * Returns: a pointer to the UArray2_T struct which represents the 2d array
 *
 ************************/
extern T UArray2_new(int col, int row, int size);

/********** UArray2_width ********
 *
 * Allocates, initializes, and returns a new 2d array of width col and length
 * row where each element occupies size bytes
 *
 * Parameters:
 *      int col: the number of columns of the 2d array
 *      int row: the number of rows in the 2d array
 *      int size: the number of bytes each element will occupy
 *
 *
 * Returns: a pointer to the UArray2_T struct which represents the 2d array
 *
 ************************/
extern int UArray2_width(T uarray2);
extern int UArray2_height(T uarray2);
extern int UArray2_size(T uarray2);
extern void *UArray2_at(T uarray2, int col, int row);

extern void UArray2_map_col_major(T uarray2, void apply(int i, int j, UArray2_T a, void *p1, void *p2), void *cl);
extern void UArray2_map_row_major(T uarray2, void apply(int i, int j, UArray2_T a, void *p1, void *p2), void *cl);

extern void UArray2_free(T *uarray2);

#endif