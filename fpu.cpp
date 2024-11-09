#include "Benchmark.h"
#include <math.h>

extern volatile int FlagLoop;

static void bitreverse(float a[],int N);

//ビット反転を行う。
static void bitreverse(float a[],int N)
{
    int i,j,k,l;
    float* b = new float[N];

    for(i=0;i<N;i++)
    {
        k=0;
        l=N/2;
        for(j=1;j<N;j*=2)
        {
            if(i&j) k+=l;
            l/=2;
        }
        b[i]=a[k];
    }
    for(i=0;i<N;i++)
        a[i] = b[i];
	delete [] b;
}

unsigned int __stdcall FFT(void *thread)
{
	_ThreadStruct* ThreadStruct = (_ThreadStruct *)thread;

	int N = (int)ThreadStruct->ExData;

	int count=0;
	const float PI = 3.14159265358979f;

    int i,j,k;
    
	float *fr = new float[N];
	float *fi = new float[N];

    float w,wr,wi,x,xr,xi;

	while(FlagLoop){
		//入力として、サイン波を使う
		for(i=0;i<N;i++)
		{
			x=-2*PI+(float)i*4.0f*PI/(float)N;
			fr[i]=(float)(sin(x) + sin(2.0f*x));
			fi[i]=0.0;
		}
	//高速フーリエ変換
		//ビット反転を行う
		bitreverse(fr,N);
		bitreverse(fi,N);
		for(k=1;k<N;k=k*2)  //  k=1,2,4,8,...   <- 手順の深さ
		{
			for(j=0;j<k;j++)
			{
				w = 2.0f*(float)PI/(2.0f*(float)k);
				wr = (float) cos(w*j);             // 回転子の複素数表現       
				wi = (float)-sin(w*j);             //
				for(i=j;i<N;i+=(k*2))
				{
					xr = wr*fr[i+k] - wi*fi[i+k];
					xi = wr*fi[i+k] + wi*fr[i+k];
					fr[i+k] = fr[i] - xr; //引く側
					fi[i+k] = fi[i] - xi;
					fr[i]   = fr[i] + xr; //足す側
					fi[i]   = fi[i] + xi;
				}
			}
		}
		count++;
	}
	delete [] fr;
	delete [] fi;

	ThreadStruct->result = (double)((double)count * N / 1153.3980);

	_endthreadex(0);
	return 0;
}

unsigned int __stdcall RandMeanSS(void *thread)
{
	_ThreadStruct* ThreadStruct = (_ThreadStruct *)thread;
	int N = (int)ThreadStruct->ExData;

	int i;
	double x,s1,s2;

	double *d = new double[N];
	static long holdrand = 1;
	int count = 0;

	while(FlagLoop){
		for(i = 0;i < N;i++){
			// C ランタイムの rand() を展開 + 少しだけ浮動小数点を利用
			//holdrand = ((holdrand * 214013L + 2531011L) >> 16) & 0x7fff;
			d[i] = (double)rand() / 1000.0; //( (1.0 / (RAND_MAX + 1.0)) * holdrand );
		}

		s1 = s2 = 0.0f;
		for(i = 1;i <= N;i++){
			x = d[i-1];
			x -= s1;
			s1 += x / i;
			s2 += (i - 1) * x * x / i;
		}
		s2 = sqrt(s2 / (N - 1));
		count++;
	}
	delete [] d;
	ThreadStruct->result = (double)(count / 40.1604);

	_endthreadex(0);
	return 0;
}

unsigned int __stdcall Mandelbrot(void *thread)
{
	_ThreadStruct* ThreadStruct = (_ThreadStruct *)thread;
	int N = (int)ThreadStruct->ExData;
	
	int num = 1;
	int count = 0;
	const int		KL = 64;// 最大繰り返し回数

	const double	RS = -2.2 * 4 / 3;// 複素平面の実部の始点
	const double	RE = 0.5 * 4 / 3;
	const double	IS = -1.35;
	const double	IE = 1.35;

	const int XS = N;
	const int YS = N;

	typedef struct _ri{
		double r;
		double i;
	}ri;

	int x,y,k,color=0;
	double dr,di;
	int boost = 0;
	ri z,z2,c;

	dr = (RE - RS) / XS;
	di = (IE - IS) / YS;

	for(;FlagLoop;){
		for(y = 0;y < YS ;y++){
			for(x = XS - 1;x >= 0;x--){				
				c.r = x * dr + RS;
				c.i = y * di + IS;
				z.i = 0.0;
				z.r = 0.0;
				for(k = 0;k < KL; k++){
					z2.r = z.r * z.r - z.i * z.i + c.r;
					z2.i = 2.0 * z.r * z.i + c.i;
					if(z2.r * z2.r + z2.i * z2.i > 4.0 ){			
						color = k * 8 << boost;
						break;
					}
					z = z2;
				}
			}
		}
		
		count++;
		boost = count % 20;
	}

	ThreadStruct->result = (double)(count / 0.012427);

	_endthreadex(0);
	return 0;
}