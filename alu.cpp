#include "Benchmark.h"

extern volatile int FlagLoop;

const float PI = 3.14159265358979f;

// エラトステネスのふるい
unsigned int __stdcall Eratosthenes(void *thread)
{
	_ThreadStruct* ThreadStruct = (_ThreadStruct *)thread;
	int N = (int)(ThreadStruct->ExData);

	int i,p,k,count = 0,cnt;
	char* flag = new char[N + 1];

	for(;FlagLoop;){
		cnt = 1;
		for(i=0;i<=N;i++){flag[i] = TRUE;}
		for(i=0;i<=N;i++){
			if(flag[i]){
				p = i + i + 3;
				for(k=i+p;k<=N;k+=p){flag[k]=FALSE;}
				cnt++;
			}
		}
		count++;
	}

	delete [] flag;

	ThreadStruct->result = (int)(count / 1000.0 / 26.4975 * N);
	
	_endthreadex(0);
	return 0;
}

// フィボナッチ数
unsigned int __stdcall Fibonacci(void *thread)
{
	_ThreadStruct* ThreadStruct = (_ThreadStruct *)thread;

	int count = 0;
	int n=0;

	int a,a1,b,b1,c,c1,x,x1,y,y1;
	for(;FlagLoop;){
		n = count + 2;
		a = 1; b = 1; c = 0; x = 1; y = 0; n--;
		while( n > 0 ){
			if(n & 1){
				x1 = x; y1=y;
				x = a * x1 + b * y1;
				y = b * x1 + c * y1;
			}
			n /= 2;
			a1 = a; b1 = b; c1 = c;
			a = a1 * a1 + b1 * b1;
			b = b1 * (a1 + c1);
			c = b1 * b1 + c1 * c1;
		}
		count++;
	}
	ThreadStruct->result = (double)(count / 910.974);

	_endthreadex(0);
	return 0;
}
