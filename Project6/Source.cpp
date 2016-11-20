#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define N 8
#define NUM_THREADS 4
int image[N][N];
int label[N][N];
int first[NUM_THREADS][N];
int second[NUM_THREADS][N];
int answer[NUM_THREADS];
int result[NUM_THREADS];
#define multiplier 5
#define stripSize N / NUM_THREADS

int imageParts[NUM_THREADS][stripSize + 2][N];
int labelParts[NUM_THREADS][stripSize + 2][N];

int main() {
	for (size_t i = 0; i < N; i++)
	{
		for (size_t j = 0; j < N; j++)
		{
			image[i][j] = rand() % 2;
			label[i][j] = i * multiplier + j;
		}
	}

	for (size_t i = 0; i < NUM_THREADS; i++)
	{
		for (size_t j = 0; j < stripSize + 2; j++)
		{
			for (size_t k = 0; k < N; k++)
			{
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
				imageParts[i][j][k] = image[i*stripSize + j - 1][k];
				labelParts[i][j][k] = label[i*stripSize + j - 1][k];
			}
		}
	}
	int i = 0;
	int chg = 1, change = 1;
	while (change)
	{
		change = 0;
		for (size_t i = 0; i < NUM_THREADS; i++)
		{
			for (size_t j = 0; j < N; j++)
			{
				first[i][j] = labelParts[i][0][j];
				second[i][j] = labelParts[i][stripSize + 1][j];
				answer[i] = 0;
			}
		}
		omp_set_num_threads(NUM_THREADS);
		int i = 0, j = 0, k = 0;
#pragma omp parallel private(i,j,k)
		{
			int id = omp_get_thread_num();
			//for (j = N - 2; j >= 0; j--)
			//{
			//	if (imageParts[id][stripSize + 1][j] == imageParts[id][stripSize + 1][j + 1])
			//	{
			//		labelParts[id][stripSize + 1][j] = labelParts[id][stripSize + 1][j + 1];
			//		answer[id] = 1;
			//	}
			//	if (imageParts[id][stripSize + 1][j] == imageParts[id][stripSize + 1][j + 1])
			//	{
			//		labelParts[id][stripSize + 1][j] = labelParts[id][stripSize + 1][j + 1];
			//		answer[id] = 1;
			//	}
			//}
			for (i = stripSize + 1; i > 0; i--)
			{
				for (j = N - 2; j >= 0; j--)
				{
					if (imageParts[id][i][j] == imageParts[id][i][j + 1])
					{
						labelParts[id][i][j] = labelParts[id][i][j + 1];
						answer[id] = 1;
					}
					if (imageParts[id][i][j] == imageParts[id][i - 1][j])
					{
						labelParts[id][i][j] = labelParts[id][i - 1][j];
						answer[id] = 1;
					}
				}
			}

		}

		for (size_t i = 0; i < NUM_THREADS; i++)
		{
			change = change || answer[i];
		}
	}

	scanf_s("%d", &i);
}