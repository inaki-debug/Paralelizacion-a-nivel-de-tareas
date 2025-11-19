#include "comb.h"



extern void all(const char *mask, const char *path) {
    
    char graymask[20], invmask[20], blurmask[20], invhmask[20];

    int mm=57;

    sprintf(graymask, "gray_%s", mask);
    sprintf(invmask,  "inv_%s",  mask);
    sprintf(blurmask, "blur_%s", mask);
    sprintf(invhmask, "invh_%s", mask);

    gray(graymask, path);
    inv_gray(invmask, path);
    blur1(mm, blurmask, path);
    inv_gray_hor(invhmask, path);

    
    
}