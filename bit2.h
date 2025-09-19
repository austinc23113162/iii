#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED

typedef struct Bit2 *Bit2_T;

extern Bit2_T Bit2_new(int width, int height);
extern void Bit2_free(Bit2_T *a);

extern int Bit2_width (Bit2_T a);
extern int Bit2_height(Bit2_T a);

extern int Bit2_get(Bit2_T a, int i, int j);
extern int Bit2_put(Bit2_T a, int i, int j, int bit);

extern void Bit2_map_row_major(
        Bit2_T a,
        void apply(int i, int j, Bit2_T b, int value, void *cl),
        void *cl);

extern void Bit2_map_col_major(
        Bit2_T a,
        void apply(int i, int j, Bit2_T b, int value, void *cl),
        void *cl);

#endif 