#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>
#include "todos.h"

//#define NUM_THREADS 6

int main(){
    double start, end;

    // Iniciar medición
    start = omp_get_wtime();

    omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
            { all("1", "./img_in/1.bmp"); }
            #pragma omp section
            { all("2", "./img_in/2.bmp"); }
            #pragma omp section
            { all("3", "./img_in/3.bmp"); }
            #pragma omp section
            { all("4", "./img_in/4.bmp"); }
            #pragma omp section
            { all("5", "./img_in/5.bmp"); }
            #pragma omp section
            { all("6", "./img_in/6.bmp"); }
            #pragma omp section
            { all("7", "./img_in/7.bmp"); }
            #pragma omp section
            { all("8", "./img_in/8.bmp"); }
            #pragma omp section
            { all("9", "./img_in/9.bmp"); }
            #pragma omp section
            { all("10", "./img_in/10.bmp"); }
            #pragma omp section
            { all("11", "./img_in/11.bmp"); }
            #pragma omp section
            { all("12", "./img_in/12.bmp"); }
            #pragma omp section
            { all("13", "./img_in/13.bmp"); }
            #pragma omp section
            { all("14", "./img_in/14.bmp"); }
            #pragma omp section
            { all("15", "./img_in/15.bmp"); }
            #pragma omp section
            { all("16", "./img_in/16.bmp"); }
            #pragma omp section
            { all("17", "./img_in/17.bmp"); }
            #pragma omp section
            { all("18", "./img_in/18.bmp"); }
            #pragma omp section
            { all("19", "./img_in/19.bmp"); }
            #pragma omp section
            { all("20", "./img_in/20.bmp"); }
            #pragma omp section
            { all("21", "./img_in/21.bmp"); }
            #pragma omp section
            { all("22", "./img_in/22.bmp"); }
            #pragma omp section
            { all("23", "./img_in/23.bmp"); }
            #pragma omp section
            { all("24", "./img_in/24.bmp"); }
            #pragma omp section
            { all("25", "./img_in/25.bmp"); }
            #pragma omp section
            { all("26", "./img_in/26.bmp"); }
            #pragma omp section
            { all("27", "./img_in/27.bmp"); }
            #pragma omp section
            { all("28", "./img_in/28.bmp"); }
            #pragma omp section
            { all("29", "./img_in/29.bmp"); }
            #pragma omp section
            { all("30", "./img_in/30.bmp"); }
        }
    }

    // Terminar medición
    end = omp_get_wtime();

    printf("\nTiempo total: %.4f segundos\n", end - start);

    return 0;
}
