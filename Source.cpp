#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <stdint.h>

unsigned int a[4096][4096];

int main() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);

	printf("Enter the number of iterations to generate gilbert's curve\niter > ");
	int iter = 4;
	int asize = 1;
	scanf("%d", &iter);
	printf("iter = %d\n", iter);

	for (int i = 0; i < iter; i++) {
		asize *= 2;
	}

	printf("asize = %d\n", asize);
	printf("a[0][0] = %u\n", a[0][0]);
	
	for (int i = 0, p = 1, n = 0, m = 1, d = 1; i < iter; i++) {
		printf("iter #%d:\t", i + 1);
		printf("->[%4d][%4d]:[%4d][%4d]->", p * n, p * m, p * n + p - 1, p * m + p - 1);
		printf("[%4d][%4d]:[%4d][%4d]->", p, p, p + p - 1, p + p - 1);
		printf("[%4d][%4d]:[%4d][%4d]\n", p * m, p * n, p * m + p - 1, p * n + p - 1);
		for (int j = p * n, j2 = 0; j < p * n + p; j++, j2++) {
			for (int k = p * m, k2 = 0; k < p * m + p; k++, k2++) {
				a[j][k] = a[k2][j2] + d;
			}
		}
		for (int j = p, j2 = 0; j < p + p; j++, j2++) {
			for (int k = p, k2 = 0; k < p + p; k++, k2++) {
				a[j][k] = a[k2][j2] + d * 2;
			}
		}
		for (int j = p * m, j2 = p - 1; j < p * m + p; j++, j2--) {
			for (int k = p * n, k2 = p - 1; k < p * n + p; k++, k2--) {
				a[j][k] = a[j2][k2] + d * 3;
			}
		}
		n = (n + 1) % 2;
		m = (m + 1) % 2;
		p = p * 2;
		d = d * 4;
	}

	printf("Writing to file...\n");

#pragma pack(push,1)
	struct bmp_h {
		uint16_t type;
		uint32_t size;
		uint16_t res1;
		uint16_t res2;
		uint32_t offset;
	};
#pragma pack(pop)

	bmp_h myHeader = {
		0x4D42,
		asize * asize * 32 / 8 + sizeof(bmp_h) + sizeof(BITMAPINFOHEADER) + 12,
		0x0000,
		0x0000,
		12 + sizeof(bmp_h) + sizeof(BITMAPINFOHEADER)
	};

	BITMAPINFOHEADER myDibHeader = {
		sizeof(myDibHeader),
		asize,
		asize,
		1,
		32,
		BI_BITFIELDS,
		0,
		0,
		0,
		0,
		0,
	};

	int rBitMask = 0x0000FF00;
	int gBitMask = 0x00FF0000;
	int bBitMask = 0x000000FF;

	FILE* fp = fopen("hilbertallcolors.bmp", "wb+");
	fwrite(&myHeader, sizeof(myHeader), 1, fp);
	fwrite(&myDibHeader, sizeof(myDibHeader), 1, fp);
	fwrite(&rBitMask, sizeof(rBitMask), 1, fp);
	fwrite(&gBitMask, sizeof(gBitMask), 1, fp);
	fwrite(&bBitMask, sizeof(bBitMask), 1, fp);
	for (int i = 0; i < asize; i++) {
		for (int j = 0; j < asize; j++) {
			fwrite(&a[i][j], sizeof(a[i][j]), 1, fp);
		}
	}
	_fcloseall();
}