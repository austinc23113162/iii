/**************************************************************
 *
 *                              uarray2.c
 *
 *     Assignment: iii (CS 40 A2)
 *     Authors:    <tvales01, >
 *     Date:       <2025-09-25>
 *
 *     Implementation of a 2-D unboxed array on top of Hanson UArray.
 *     Representation is an array-of-columns: an outer UArray_T of
 *     length width; each slot holds an inner UArray_T of length height
 *     with element size == size. Mapping orders match the spec.
 *
 *     Dependencies:
 *       assert.h (Hanson), mem.h (NEW/FREE), uarray.h (Hanson).
 *
 *     Indices and order:
 *       i = column, j = row.
 *       Row-major: j outer (0..height-1), i inner (0..width-1).
 *       Col-major: i outer (0..width-1), j inner (0..height-1).
 *
 *     Representation invariant (assumed on entry; re-established on
 *     return):
 *       width >= 0; height >= 0; size > 0.
 *       UArray_size(outer_arr) == sizeof(UArray_T).
 *       For each column i: inner != NULL (unless height==0),
 *         UArray_length(inner) == height, UArray_size(inner) == size.
 *
 *     Checked runtime errors (CREs):
 *       UArray2_new: width<0 || height<0 || size<=0.
 *       UArray2_at / maps: NULL handle, OOB indices, NULL apply.
 *       UArray2_free: NULL pointer or *ptr==NULL.
 *
 **************************************************************/

#include "uarray2.h"
#include "uarray.h"
#include "assert.h"
#include "mem.h"

struct UArray2_T {
        int width;
        int height;
        int size;
        UArray_T outer_arr;
};

/********** UArray2_new ********
 * Create a 2-D unboxed array with elements of size `size`.
 *
 * Parameters:
 *      int col:   width  (#columns)  >= 0
 *      int row:   height (#rows)     >= 0
 *      int size:  element size in bytes (> 0)
 *
 * Returns:
 *      UArray2_T: new array with uninitialized element storage
 *
 * Effects:
 *      Allocates an outer UArray_T of length col, each slot holds an inner
 *      UArray_T of length row with element size `size`.
 *
 * CRE
 *      CRE if col < 0 or row < 0 or size <= 0
 *      May CRE on allocation failure
 ************************/
UArray2_T UArray2_new(int col, int row, int size)
{
        UArray2_T uarray2;
        NEW(uarray2);

        assert(col >= 0 && row >= 0 && size > 0);
        uarray2->width = col;
        uarray2->height = row;
        uarray2->size = size;
        uarray2->outer_arr = UArray_new(col, sizeof(UArray_T));

        for (int i = 0; i < col; i++) {
                UArray_T inner_arr = UArray_new(row, size);

                UArray_T *innerp = UArray_at(uarray2->outer_arr, i);
                *innerp = inner_arr;
        }

        return uarray2;
}

/********** UArray2_width / UArray2_height / UArray2_size ********
 * Return array dimensions and element size.
 *
 * Parameters:
 *      UArray2_T a: non-NULL
 *
 * Returns:
 *      width (columns) / height (rows) / element size in bytes
 *
 * CRE
 *      CRE if a == NULL
 ************************/
int UArray2_width(UArray2_T uarray2)
{
        assert(uarray2);
        return uarray2->width;
}

int UArray2_height(UArray2_T uarray2)
{
        assert(uarray2);
        return uarray2->height;
}

int UArray2_size(UArray2_T uarray2)
{
        assert(uarray2);
        return uarray2->size;
}

/********** UArray2_at ********
 * Return a pointer to the element at (col,row).
 *
 * Parameters:
 *      UArray2_T a: non-NULL array
 *      int col:     0 <= col < width
 *      int row:     0 <= row < height
 *
 * Returns:
 *      void *: address of element storage, valid until array is freed
 *
 * CRE
 *      CRE if a == NULL or indices out of bounds
 ************************/
void *UArray2_at(UArray2_T uarray2, int col, int row)
{
        assert(uarray2);
        assert(col >= 0 && col < uarray2->width);
        assert(row >= 0 && row < uarray2->height);

        UArray_T *innerp = UArray_at(uarray2->outer_arr, col);
        UArray_T inner = *innerp;
        assert(sizeof(inner) == UArray_size(uarray2->outer_arr));
        assert(UArray_size(inner) == uarray2->size);

        return UArray_at(inner, row);
}

/********** UArray2_map_row_major ********
 * Visit every element in row-major order and call apply for each.
 *
 * Parameters:
 *      UArray2_T a: array
 *      void apply(int col, int row, UArray2_T a2, void *elem, void *cl):
 *                   client callback; `elem` points to element at (col,row)
 *      void *cl:    closure passed through
 *
 * Returns:
 *      None
 *
 * Order:
 *      Rows outermost (0..height-1), columns innermost (0..width-1).
 *
 * CRE
 *      CRE if a == NULL or apply == NULL
 ************************/
void UArray2_map_row_major(UArray2_T uarray2,
                                  void apply(int col, int row, UArray2_T a,
                                             void *p1, void *p2),
                                  void *cl) 
{
        assert(uarray2);
        UArray_T outer_arr = uarray2->outer_arr;

        for (int row = 0; row < uarray2->height; row++) {
                for (int col = 0; col < uarray2->width; col++) {
                        UArray_T *innerp = UArray_at(outer_arr, col);
                        UArray_T inner = *innerp;
                        assert(sizeof(inner) == UArray_size(outer_arr));
                        apply(col, row, uarray2, UArray_at(inner, row), cl);
                }
        }                         
}

/********** UArray2_map_col_major ********
 * Same as above but column-major: columns outermost, rows innermost.
 ************************/
void UArray2_map_col_major(UArray2_T uarray2,
                                  void apply(int col, int row, UArray2_T a,
                                             void *p1, void *p2),
                                  void *cl)
{
        assert(uarray2);
        UArray_T outer_arr = uarray2->outer_arr;

        for (int col = 0; col < uarray2->width; col++) {
                UArray_T *innerp = UArray_at(outer_arr, col);
                UArray_T inner = *innerp;
                assert(sizeof(inner) == UArray_size(outer_arr));

                for (int row = 0; row < uarray2->height; row++) {
                        apply(col, row, uarray2, UArray_at(inner, row), cl);
                }
        }

}

/********** UArray2_free ********
 * Free all storage and set *uarray2 to NULL.
 *
 * Parameters:
 *      UArray2_T *uarray2: pointer to handle; *uarray2 must be non-NULL
 *
 * Returns:
 *      None
 *
 * Effects:
 *      Frees all inner arrays, the outer array, and the header; sets *uarray2=NULL.
 *
 * CRE
 *      CRE if uarray2 == NULL or *uarray2 == NULL
 ************************/
void UArray2_free(UArray2_T *uarray2) {
        assert(uarray2 && *uarray2);

        for (int col = 0; col < (*uarray2)->width; col++) {
                UArray_T *innerp = UArray_at((*uarray2)->outer_arr, col);
                UArray_free(innerp);
        }
        UArray_free(&(*uarray2)->outer_arr);
        FREE(*uarray2);
}
