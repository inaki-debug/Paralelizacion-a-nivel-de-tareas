#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

#define NUM_THREADS 50

extern void gray(const char *mask, const char *path)
{
    FILE *image, *outputImage;    
    char add_char[80] = "./img_out/";
    strcat(add_char, mask);
    strcat(add_char, ".bmp");
    printf("%s\n", add_char);

    image = fopen(path,"rb");
    outputImage = fopen(add_char,"wb");

    if(!image || !outputImage){
        printf("Error abriendo archivos\n");
        return;
    }

    for(int i = 0; i < 54; i++)
        fputc(fgetc(image), outputImage);

    fseek(image, 0, SEEK_END);
    long fileSize = ftell(image);
    long pixelDataSize = fileSize - 54;
    fseek(image, 54, SEEK_SET);

    unsigned char *buffer = (unsigned char*) malloc(pixelDataSize);
    fread(buffer, 1, pixelDataSize, image);

    unsigned char *grayBuffer = (unsigned char*) malloc(pixelDataSize);

    long numPixels = pixelDataSize / 3;

    #pragma omp parallel for
    for(long i = 0; i < numPixels; i++){
        unsigned char b = buffer[3*i + 0];
        unsigned char g = buffer[3*i + 1];
        unsigned char r = buffer[3*i + 2];

        unsigned char pixel = 0.21*r + 0.72*g + 0.07*b;

        grayBuffer[3*i + 0] = pixel;
        grayBuffer[3*i + 1] = pixel;
        grayBuffer[3*i + 2] = pixel;
    }

    fwrite(grayBuffer, 1, pixelDataSize, outputImage);

    free(buffer);
    free(grayBuffer);
    fclose(image);
    fclose(outputImage);
}



extern void inv_gray(const char *mask, const char *path){
    FILE *image = NULL, *outputImage = NULL;
    char add_char[80] = "./img_out/";
    strcat(add_char, mask);
    strcat(add_char, ".bmp");
    printf("%s\n", add_char);

    image = fopen(path, "rb");
    outputImage = fopen(add_char, "wb");

    long ancho=0, alto=0, tam=0, bpp=0;
    unsigned char xx[54];

    for(int i = 0; i < 54; i++){
      int c = fgetc(image);
      xx[i] = (unsigned char)c;
      fputc(xx[i], outputImage);
    }

    tam   = (long)xx[2] | ((long)xx[3] << 8) | ((long)xx[4] << 16) | ((long)xx[5] << 24);
    bpp   = (long)xx[28] | ((long)xx[29] << 8);
    ancho = (long)xx[18] | ((long)xx[19] << 8) | ((long)xx[20] << 16) | ((long)xx[21] << 24);
    alto  = (long)xx[22] | ((long)xx[23] << 8) | ((long)xx[24] << 16) | ((long)xx[25] << 24);

    int bytesPerPixel = 3;
    int rowSize = ((ancho * bytesPerPixel + 3) / 4) * 4; 
    size_t pixelArea = (size_t)rowSize * (size_t)alto;

    unsigned char *rowBuf = malloc(rowSize);

    size_t npix = (size_t)ancho * (size_t)alto;
    unsigned char *arr_b = malloc(npix);
    unsigned char *arr_g = malloc(npix);
    unsigned char *arr_r = malloc(npix);

    for (long row = 0; row < alto; row++) {
        size_t read = fread(rowBuf, 1, rowSize, image);
        if (read < (size_t)rowSize) {
            for (size_t k = read; k < (size_t)rowSize; k++) rowBuf[k] = 0;
        }
        for (long col = 0; col < ancho; col++) {
            size_t idxRow = (size_t)row * (size_t)ancho + (size_t)col;
            size_t off = (size_t)col * bytesPerPixel;
            arr_b[idxRow] = rowBuf[off + 0];
            arr_g[idxRow] = rowBuf[off + 1];
            arr_r[idxRow] = rowBuf[off + 2];
        }
    }

    free(rowBuf);

    unsigned char *outBuf = malloc(npix * 3);

    #pragma omp parallel for
    for (size_t i = 0; i < npix; i++) {
        size_t inv_i = npix - 1 - i;
        unsigned char gray = (unsigned char)(
            0.299f * arr_r[inv_i] +
            0.587f * arr_g[inv_i] +
            0.114f * arr_b[inv_i]
        );

        outBuf[3*i+0] = gray;
        outBuf[3*i+1] = gray;
        outBuf[3*i+2] = gray;
    }

    fwrite(outBuf, 1, npix*3, outputImage);

    free(outBuf);
    free(arr_b);
    free(arr_g);
    free(arr_r);

    fclose(image);
    fclose(outputImage);
}





void blur1(int mm, const char *mask, const char *path)
{
    FILE *image = fopen(path, "rb");

    char add_char[80] = "./img_out/";
    char mma[20];
    strcat(add_char, mask);
    sprintf(mma, "%d", mm);
    strcat(add_char, mma);
    strcat(add_char, ".bmp");

    FILE *outputImage = fopen(add_char, "wb");

    long ancho=0, alto=0, tam=0;
    unsigned char xx[54];
    for(int i=0; i<54; i++) {
      int c = fgetc(image);
      xx[i] = (unsigned char)c;
      fputc(xx[i], outputImage); 
    }

    tam   = (long)xx[2] | ((long)xx[3] << 8) | ((long)xx[4] << 16) | ((long)xx[5] << 24);
    ancho = (long)xx[18] | ((long)xx[19] << 8) | ((long)xx[20] << 16) | ((long)xx[21] << 24);
    alto  = (long)xx[22] | ((long)xx[23] << 8) | ((long)xx[24] << 16) | ((long)xx[25] << 24);

    printf("Inicio de bluring\n");
    printf("largo img %li\n", alto);
    printf("ancho img %li\n", ancho);

    int bytesPerPixel = 3;
    int rowSize = ((ancho * bytesPerPixel + 3) / 4) * 4;
    size_t pixelDataSize = (size_t)rowSize * (size_t)alto;

    unsigned char *pixelData = malloc(pixelDataSize);

    size_t offset = 0;
    for (long row = 0; row < alto; row++) {
        size_t got = fread(pixelData + offset, 1, rowSize, image);
        if (got < (size_t)rowSize) {
            for (size_t z = got; z < (size_t)rowSize; z++) pixelData[offset + z] = 0;
        }
        offset += rowSize;
    }

    printf("Inicia procesamiento\n");

    int kernelRadius = (mm - 1) / 2;

    float progreso;
        #pragma omp parallel for
        for (long y = 0; y < alto; y++)
        {
            if (y % 100 == 0) {
               printf("\x1b[1F");
               progreso = ((float)y/(float)alto)*100.0f;
               printf("\r Avance %0.2f / 100 \n", progreso);
            }

            for (long x = 0; x < ancho; x++)
            {
                unsigned int rSum = 0, gSum = 0, bSum = 0;
                int pond = 0;

                for (int ky = -kernelRadius; ky <= kernelRadius; ky++){
                    for (int kx = -kernelRadius; kx <= kernelRadius; kx++){
                        long py = y + ky;
                        long px = x + kx;
                        if (py < 0 || py >= alto || px < 0 || px >= ancho) continue;
                        size_t index = (size_t)py * (size_t)rowSize + (size_t)px * bytesPerPixel;
                        if (index + 2 >= pixelDataSize) continue;
                        bSum += pixelData[index + 0];
                        gSum += pixelData[index + 1];
                        rSum += pixelData[index + 2];
                        pond++;
                    }
                }

                size_t idxOut = (size_t)y * (size_t)rowSize + (size_t)x * bytesPerPixel;
                if (pond > 0 && idxOut + 2 < pixelDataSize) {
                    pixelData[idxOut + 0] = (unsigned char)(bSum / pond);
                    pixelData[idxOut + 1] = (unsigned char)(gSum / pond);
                    pixelData[idxOut + 2] = (unsigned char)(rSum / pond);
                }
            }
        }
    

    printf("\x1b[1F");
    printf("\r Avance 100 / 100 \n");
    printf("Fin procesamiento\n");

    fwrite(pixelData, 1, pixelDataSize, outputImage);

    free(pixelData);
    fclose(image);
    fclose(outputImage);
}

extern void inv_gray_hor(const char *mask, const char *path) {
    FILE *image = NULL, *outputImage = NULL;
    char add_char[80] = "./img_out/";
    strcat(add_char, mask);
    strcat(add_char, ".bmp");
    printf("%s\n", add_char);

    image = fopen(path, "rb");
    outputImage = fopen(add_char, "wb");

    long ancho=0, alto=0, tam=0, bpp=0;
    unsigned char xx[54];

    for(int i = 0; i < 54; i++){
        int c = fgetc(image);
        xx[i] = (unsigned char)c;
        fputc(xx[i], outputImage);
    }

    tam   = (long)xx[2] | ((long)xx[3] << 8) | ((long)xx[4] << 16) | ((long)xx[5] << 24);
    bpp   = (long)xx[28] | ((long)xx[29] << 8);
    ancho = (long)xx[18] | ((long)xx[19] << 8) | ((long)xx[20] << 16) | ((long)xx[21] << 24);
    alto  = (long)xx[22] | ((long)xx[23] << 8) | ((long)xx[24] << 16) | ((long)xx[25] << 24);

    int bytesPerPixel = 3;
    int rowSize = ((ancho * bytesPerPixel + 3) / 4) * 4;

    unsigned char *rowBuf = malloc(rowSize);

    size_t npix = (size_t)ancho * (size_t)alto;
    unsigned char *arr_b = malloc(npix);
    unsigned char *arr_g = malloc(npix);
    unsigned char *arr_r = malloc(npix);

    for (long row = 0; row < alto; row++) {
        size_t read = fread(rowBuf, 1, rowSize, image);
        if (read < (size_t)rowSize) {
            for (size_t k = read; k < (size_t)rowSize; k++) rowBuf[k] = 0;
        }

        for (long col = 0; col < ancho; col++) {
            size_t idx = (size_t)row * (size_t)ancho + (size_t)col;
            size_t off = (size_t)col * bytesPerPixel;

            arr_b[idx] = rowBuf[off + 0];
            arr_g[idx] = rowBuf[off + 1];
            arr_r[idx] = rowBuf[off + 2];
        }
    }

    free(rowBuf);

    unsigned char *outBuf = malloc(npix * 3);

    #pragma omp parallel for
    for (long row = 0; row < alto; row++) {
        for (long col = 0; col < ancho; col++) {

            long col_inv = ancho - 1 - col;
            size_t idx = (size_t)row * (size_t)ancho + (size_t)col_inv;

            unsigned char gray = (unsigned char)(
                0.299f * arr_r[idx] +
                0.587f * arr_g[idx] +
                0.114f * arr_b[idx]
            );

            size_t out_i = (size_t)row * (size_t)ancho + (size_t)col;
            outBuf[3*out_i+0] = gray;
            outBuf[3*out_i+1] = gray;
            outBuf[3*out_i+2] = gray;
        }
    }

    fwrite(outBuf, 1, npix*3, outputImage);

    free(outBuf);
    free(arr_b);
    free(arr_g);
    free(arr_r);

    fclose(image);
    fclose(outputImage);
}




