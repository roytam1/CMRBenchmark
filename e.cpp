#include "Benchmark.h"

extern volatile int FlagLoop;

// ‘½”{’·‰‰Z //

#define RADIXBITS 15
#define RADIX (1U << RADIXBITS)

#define N 225
#define M 250

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

static void add(ushort a[],ushort b[],ushort c[])
{
	int i;
	uint u = 0;

	for (i = N; i >= 0; i--){
		u += a[i] + b[i];
		c[i] = u & (RADIX - 1);
		u >>= RADIXBITS;
	}
}

static void sub(ushort a[],ushort b[],ushort c[])
{
	int i;
	uint u = 0;

	for (i = N; i >= 0; i--){
		u = a[i] - b[i] - u;
		c[i] = u & (RADIX - 1);
		u = (u >>= RADIXBITS) & 1;
	}
}

static void muls(ushort a[],uint x,ushort b[])
{
	int i;
	ulong t = 0;

	for (i = N; i >= 0; i--){
		t += (ulong) a[i] * x;
		b[i] = (uint) t & (RADIX - 1);
		t >>= RADIXBITS;
	}
}

static int divs(int m,ushort a[],uint x,ushort b[])
{
	int i;
	ulong t = 0;

	for (i = m; i <= N; i++){
		t = (t << RADIXBITS) + a[i];
		b[i] = (ushort) t / x; t %= x;
	}

	if(2 * t >= x){
		for(i = N; ++b[i] & RADIX; i--){
			b[i] &= RADIX -1;
		}
	}
	return (b[m] != 0) ? m : (m + 1);
}

// ©‘R‘Î”‚Ì’ê e ‚ğ‹‚ß‚é
unsigned int __stdcall Napierian(void *thread)
{
	_ThreadStruct* ThreadStruct = (_ThreadStruct *)thread;
	int count = 0;
	ushort *a = new ushort[N+1];
	ushort *t = new ushort[N+1];

	while(FlagLoop){
		int m;
		uint k;
		for (m =0; m <= N; m++){
			a[m] = t[m] = 0;
		}
		a[0] = 2;
		a[1] = t[1] = RADIX / 2;
		k = 3; m = 1;
		while ( (m = divs(m,t,k,t)) <= N ){
			add(a,t,a);
			++k;
			// if(++k ==  RADIX){error("Œ…”‚ª‘½‚·‚¬‚Ü‚·");}
		}
	//	print(a);
		count++;
	}
	delete [] t;
	delete [] a;
	ThreadStruct->result = (int)(count / 0.3197);

	_endthreadex(0);
	return 0;
}