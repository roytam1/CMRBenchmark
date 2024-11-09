#pragma once

#include <windows.h>
#include <process.h>

typedef struct __ThreadData
{
	void* ExData;
	double result;
}_ThreadStruct;

int benchmark(int testNo, int testThreads, void* exData, int* score);

unsigned int __stdcall Fibonacci(void* thread);
unsigned int __stdcall Napierian(void* thread);
unsigned int __stdcall Eratosthenes(void* thread);
unsigned int __stdcall FFT(void* thread);
unsigned int __stdcall RandMeanSS(void* thread);
unsigned int __stdcall Mandelbrot(void* thread);