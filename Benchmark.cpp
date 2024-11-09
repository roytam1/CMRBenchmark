
#include "Benchmark.h"

//#include <iostream>
#include <stdio.h>
#include <process.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

volatile int FlagLoop;
unsigned int (__stdcall *FUNC)(void*);

int main(int argc, char** argv)
{
	//int argc = 0;
	int testNo = -1;
	int testThreads = -1;
	int score = 0;

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numCPU = sysinfo.dwNumberOfProcessors;
	//printf("numCPU = %d\n", numCPU);

	//LPSTR* argv = CommandLineToArgvA(GetCommandLineA(), &argc);

	if (argc == 3)
	{
		testNo = atoi(argv[1]);
		testThreads = atoi(argv[2]);
	}
	else
	{
		testNo = 4;
		testThreads = numCPU;
	}

	if (testNo == -1 || testThreads == -1)
	{
		return -1;
	}

	int n = 0;
	switch (testNo)
	{
	case 1: n = 0;   break; // Fibonacci
	case 2: n = 0;	 break; // Napierian
	case 3:	n = 1024;break; // Eratosthenes
	case 4:	n = 128; break; // RandMeanSS
	case 5:	n = 128; break; // FFT
	case 6:	n = 256; break; // Mandelbrot
	default: return -1;
	}

	benchmark(testNo, testThreads, (void*)n, &score);

    return score;
}

int benchmark(int testNo, int testThreads, void* exData, int* score)
{
	double result = 0.0;
	const int BenchTime = 10000;
	int i;

	_ThreadStruct* ThreadStruct = new _ThreadStruct[testThreads];
	for (i = 0; i < testThreads; i++) {
		ThreadStruct[i].ExData = (void*)exData;
		ThreadStruct[i].result = 0.0;
	}
	HANDLE* hThread = new HANDLE[testThreads];

	LARGE_INTEGER lPre, lStart, lEnd, lFrequency;
	DWORD StartTime = 0, EndTime = 0;
	double totalTime = 0.0;

	FlagLoop = TRUE;

	// for DEBUG
	printf("testNo = %d\n", testNo);
	printf("testThreads = %d\n", testThreads);

	// スタート時間の確認 //
	StartTime = timeGetTime();
	QueryPerformanceFrequency(&lFrequency);
	QueryPerformanceCounter(&lPre);
	QueryPerformanceCounter(&lStart);

	switch (testNo)
	{
	case 1: FUNC = Fibonacci;		break;
	case 2: FUNC = Napierian;		break;
	case 3:	FUNC = Eratosthenes;	break;
	case 4:	FUNC = RandMeanSS;		break;
	case 5:	FUNC = FFT;				break;
	case 6:	FUNC = Mandelbrot;		break;
	default: return -1;
	}

	for (i = 0; i < testThreads; i++) {
		hThread[i] = (unsigned long*)_beginthreadex(NULL, 0, FUNC, (void*)&ThreadStruct[i], 0, NULL);
	}

	while (StartTime + BenchTime > timeGetTime()) {	Sleep(100); }
	FlagLoop = FALSE;

	//	for(i=0;i < NumThread;i++){SetThreadPriority(hThread[i],THREAD_PRIORITY_HIGHEST);}

	WaitForMultipleObjects(testThreads, hThread, TRUE, INFINITE);

	// 終了時間の確認 //
	QueryPerformanceCounter(&lEnd);
	EndTime = timeGetTime();

	// 経過時間
	double totalTimeQPC = (double)((lEnd.QuadPart - lStart.QuadPart) - (lStart.QuadPart - lPre.QuadPart)) / (double)(lFrequency.QuadPart);// 秒単位に変換
	double totalTimeWT = (double)(EndTime - StartTime) / 1000.0;// 秒単位に変換

	const double ERROR_RATE = (double)BenchTime / 1000.0 / 10;
	if (totalTimeWT - ERROR_RATE < totalTimeQPC && totalTimeQPC < totalTimeWT + ERROR_RATE) {
		totalTime = totalTimeQPC;
	}
	else {
		totalTime = totalTimeWT;
	}

	// デバッグ用測定時間表示ダイアログボックス
	// TCHAR str[256];
	// wsprintfW(str, L"%5.4f", totalTime);
	// MessageBox(NULL, str, str,MB_OK);

	for (i = 0; i < testThreads; i++)
	{
	//	wprintf(L"%02d:%f\n", i, ThreadStruct[i].result);
		result += ThreadStruct[i].result;

		if(hThread[i] != NULL)
		{
			CloseHandle(hThread[i]);
		}
	}

	delete[] ThreadStruct;
	delete[] hThread;

	// for DEBUG
	printf("result = %f\n", result);
	printf("totalTime = %d ms\n", (int)(totalTime*1000));
	printf("score = %d\n", (int)(result / totalTime));

	//printf("%d\n", (int)(result / totalTime));

	*score = (int)(result / totalTime);

	return *score;
}
