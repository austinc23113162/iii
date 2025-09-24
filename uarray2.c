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


void UArray2_free(UArray2_T *uarray2) {
        assert(uarray2 && *uarray2);

        for (int col = 0; col < (*uarray2)->width; col++) {
                UArray_T *innerp = UArray_at((*uarray2)->outer_arr, col);
                UArray_free(innerp);
        }
        UArray_free(&(*uarray2)->outer_arr);
        FREE(*uarray2);
}
