#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <time.h>

#define N 1024
#define NUM_THREADS 1

#define multiplier 5
#define stripSize N / NUM_THREADS

#define DEBUG 0


void printAnswers();

void printLabelPartForThread(int id);

void printLabelParts();

void printImage();

void printLabel();

void devideByParts();

void find();

int **allocate2dArray(int **arrayPointer, int m, int n);

int ***allocate3dArray(int ***arrayPointer, int x, int y, int z);

void fillLabel();

int **image, **label, **first, **second,
        ***imageParts, ***labelParts,
        *answer;

int main() {
    image = allocate2dArray(image, N, N);
    label = allocate2dArray(label, N, N);
    first = allocate2dArray(first, NUM_THREADS, N);
    second = allocate2dArray(second, NUM_THREADS, N);

    answer = new int[NUM_THREADS]();
    imageParts = allocate3dArray(imageParts, NUM_THREADS, stripSize + 2, N);
    labelParts = allocate3dArray(labelParts, NUM_THREADS, stripSize + 2, N);
//    int imageParts[NUM_THREADS][stripSize + 2][N];
//    int labelParts[NUM_THREADS][stripSize + 2][N];

    srand(1);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            image[i][j] = rand() % 2;
        }
    }
    fillLabel();
    devideByParts();
    if (DEBUG) {
        printImage();
        printLabel();
        printLabelParts();
    }
    omp_set_num_threads(NUM_THREADS);
    find();
    long start = clock();
    find();
    long stop = clock();
    int n = NUM_THREADS;
    printf("\nTime for %d threads %li\n", n, (stop - start));
//
//    fillLabel();
//    devideByParts();
//    omp_set_num_threads(4);
//
//    start = clock();
//    find();
//    stop = clock();
//    n = 4;
//    printf("\nTime for %d threads %li\n", n, (stop - start));
//
//    fillLabel();
//    devideByParts();
//    omp_set_num_threads(2);
//
//    start = clock();
//    find();
//    stop = clock();
//    n = 2;
//    printf("\nTime for %d threads %li\n", n, (stop - start));
//
//    fillLabel();
//    devideByParts();
//    omp_set_num_threads(1);
//
//    start = clock();
//    find();
//    stop = clock();
//    n = 1;
//    printf("\nTime for %d threads %li\n", n, (stop - start));

    return 0;
}

void fillLabel() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            label[i][j] = i * multiplier + j;
        }
    }
}

int ***allocate3dArray(int ***arrayPointer, int x, int y, int z) {
    arrayPointer = new int **[x]();
    for (int i = 0; i < x; ++i) {
        arrayPointer[i] = new int *[y]();
        for (int j = 0; j < y; ++j) {
            arrayPointer[i][j] = new int[z]();
        }
    }
    return arrayPointer;
}

int **allocate2dArray(int **arrayPointer, int m, int n) {
    arrayPointer = new int *[m]();
    for (int k = 0; k < m; ++k) {
        arrayPointer[k] = new int[n]();
    }
    return arrayPointer;
}

void find() {
    int iteration = 0;

    int change = 1;
    while (change) {
        iteration++;
        change = 0;
        for (int i = 1; i < NUM_THREADS; i++) {
            for (int j = 0; j < N; j++) {
                first[i][j] = labelParts[i - 1][0][j];
            }
            answer[i] = 0;
        }
        for (int i = 0; i < NUM_THREADS - 1; i++) {
            for (int j = 0; j < N; j++) {
                second[i][j] = labelParts[i + 1][stripSize + 1][j];
            }
            answer[i] = 0;
        }
        answer[NUM_THREADS - 1] = 0;
        if (DEBUG) {
            printLabelParts();
        }
        int i = 0, j = 0, k = 0;
#pragma omp parallel private(i,j,k)
        {
            int id = omp_get_thread_num();

            if (id != 0) {
                for (k = 0; k < N; ++k) {
                    labelParts[id - 1][stripSize + 1][k] = labelParts[id][1][k];
                }
            }
            if (id != NUM_THREADS - 1) {
                for (k = 0; k < N; ++k) {
                    labelParts[id + 1][0][k] = labelParts[id][stripSize][k];
                }
            }
#pragma omp barrier
            if (DEBUG) {
                printLabelPartForThread(id);
            }
            for (i = stripSize + 1; i > 0; i--) {
                for (j = N - 1; j >= 0; j--) {
                    int topIm = imageParts[id][i][j] == imageParts[id][i - 1][j];
                    int topLab = labelParts[id][i][j] != labelParts[id][i - 1][j];
                    int top = topIm & topLab;
                    if (top) {
                        int max = std::max(labelParts[id][i - 1][j], labelParts[id][i][j]);
                        labelParts[id][i - 1][j] = max;
                        labelParts[id][i][j] = max;
                        answer[id] = 1;
                    }
                }
            }

            for (i = stripSize + 1; i > 0; i--) {
                for (j = N - 2; j >= 0; j--) {
                    int rightIm = imageParts[id][i][j] == imageParts[id][i][j + 1];
                    int rightLab = labelParts[id][i][j] != labelParts[id][i][j + 1];
                    int right = rightIm & rightLab;
                    if (right) {
                        int max = std::max(labelParts[id][i][j], labelParts[id][i][j + 1]);
                        labelParts[id][i][j] = max;
                        labelParts[id][i][j + 1] = max;
                        answer[id] = 1;
                    }
                }
            }

        }
        for (i = 0; i < NUM_THREADS; i++) {
            change = change || answer[i];
        }
        if (DEBUG) {
            printAnswers();
        }
    }
    for (int i = 0; i < NUM_THREADS; ++i) {
        for (int j = 0; j < stripSize; ++j) {
            for (int k = 0; k < N; ++k) {
                label[i * stripSize + j][k] = labelParts[i][j + 1][k];
            }
        }
    }
    if (DEBUG) {
        printLabel();
        printLabelParts();
    }
}

void devideByParts() {
    for (int i = 0; i < NUM_THREADS; i++) {
        for (int j = 0; j < stripSize + 2; j++) {
            for (int k = 0; k < N; k++) {
                if (j == 0) {
                    if (i == 0) {
                        imageParts[i][j][k] = -1;
                        continue;
                    }
                }
                if (j == stripSize + 1) {
                    if (i == NUM_THREADS - 1) {
                        imageParts[i][j][k] = -1;
                        continue;
                    }
                }
                imageParts[i][j][k] = image[i * stripSize + j - 1][k];
                labelParts[i][j][k] = label[i * stripSize + j - 1][k];
            }
        }
    }
}

void printImage() {
    printf("---------------------------IMAGE------------------------------\n");
    for (int i1 = 0; i1 < N; ++i1) {
        for (int i = 0; i < N - 1; ++i) {
            printf("%d ", image[i1][i]);
        }
        printf("%d\n", image[i1][N - 1]);
    }
    printf("---------------------------IMAGE------------------------------\n");
}

void printLabel() {
    printf("---------------------------LABEL-----------------------------\n");
    for (int i1 = 0; i1 < N; ++i1) {
        for (int i = 0; i < N - 1; ++i) {
            printf("%-2d ", label[i1][i]);
        }
        printf("%-2d\n", label[i1][N - 1]);
    }
    printf("---------------------------LABEL-----------------------------\n");
}

void printLabelParts() {
    printf("-----------------------------LABEL PARTS--------------------------------------\n");
    for (int l = 0; l < NUM_THREADS; ++l) {
        for (int m = 0; m < stripSize + 2; ++m) {
            for (int n = 0; n < N - 1; ++n) {
                printf("%-2d ", labelParts[l][m][n]);
            }
            printf("%-2d\n", labelParts[l][m][N - 1]);
        }
        printf("\n");
    }
}

void printLabelPartForThread(int id) {
#pragma omp critical
    {
        printf("-----------------------------LABEL PART %d--------------------------------------\n", id);
        for (int m = 0; m < stripSize + 2; ++m) {
            for (int n = 0; n < N - 1; ++n) {
                printf("%-2d ", labelParts[id][m][n]);
            }
            printf("%-2d\n", labelParts[id][m][stripSize + 1]);
        }
        printf("\n");
    }
}

void printAnswers() {
    printf("\n -------------------------------------ANSWERS------------------------------------\n");

    for (int i = 0; i < NUM_THREADS; i++) {
        printf("%d ", answer[i]);
    }
    printf("\n");
}
