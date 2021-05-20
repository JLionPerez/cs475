#include <omp.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <xmmintrin.h>

#define SSE_WIDTH		4

#ifndef	NUMTRIES
#define NUMTRIES		1000
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE		1000
#endif

void SimdMul( float *a, float *b,   float *c,   int len );
float SimdMulSum( float *a, float *b, int len );
float nonSimdMulSum(float * a, float * b, int len);
float * createArray(int length);

int main() {
    float * a = createArray(ARRAY_SIZE);
    float * b = createArray(ARRAY_SIZE);

	float maxSIMDPerform = 0.0;
    float maxNonSIMDPerform = 0.0;

	for(int i = 0; i < NUMTRIES; i++) {
        double time_0 = omp_get_wtime();
        float temp = SimdMulSum(a, b, ARRAY_SIZE);
        double time_1 = omp_get_wtime();
        
        double megaMulsPerSecond_SIMD = (float)ARRAY_SIZE / (time_1 - time_0) / 1000000.0;
        if (megaMulsPerSecond_SIMD > maxSIMDPerform){
			maxSIMDPerform = megaMulsPerSecond_SIMD;
		}
        
        double time_2 = omp_get_wtime();
        float temp_2 = nonSimdMulSum(a, b, ARRAY_SIZE);
        double time_3 = omp_get_wtime();
        
        double megaMulsPerSecond_nonSIMD = (float)ARRAY_SIZE / (time_3 - time_2) / 1000000.0;
        if (megaMulsPerSecond_nonSIMD > maxNonSIMDPerform){
			maxNonSIMDPerform = megaMulsPerSecond_nonSIMD;
		}
	}

	float S = maxSIMDPerform / maxNonSIMDPerform;
	printf("%d, %lf, %lf, %lf\n", ARRAY_SIZE, maxSIMDPerform, maxNonSIMDPerform, S);

    delete[] a;
    delete[] b;

	return 0;
}

void
SimdMul( float *a, float *b,   float *c,   int len )
{
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	register float *pa = a;
	register float *pb = b;
	register float *pc = c;
	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		_mm_storeu_ps( pc,  _mm_mul_ps( _mm_loadu_ps( pa ), _mm_loadu_ps( pb ) ) );
		pa += SSE_WIDTH;
		pb += SSE_WIDTH;
		pc += SSE_WIDTH;
	}

	for( int i = limit; i < len; i++ )
	{
		c[i] = a[i] * b[i];
	}
}


float
SimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	register float *pa = a;
	register float *pb = b;

	__m128 ss = _mm_loadu_ps( &sum[0] );
	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		ss = _mm_add_ps( ss, _mm_mul_ps( _mm_loadu_ps( pa ), _mm_loadu_ps( pb ) ) );
		pa += SSE_WIDTH;
		pb += SSE_WIDTH;
	}
	_mm_storeu_ps( &sum[0], ss );

	for( int i = limit; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
}

float nonSimdMulSum(float * a, float * b, int len) {
	float sum = 0.0;

    for( int i = 0; i < len; i++){
        sum += a[i] * b[i];
    }

   return sum;	
}

float * createArray(int len){
    srand(time(0));
    
    float * num = (float *)malloc(sizeof(float) * len);
    
    for (int i = 0; i < len; i++) {
        num[i] = ((float)rand()/(float)(RAND_MAX)) * 100;
    }

    return num;
}