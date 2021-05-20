#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define _USE_MATH_DEFINES

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

#define N	0.7

// setting the number of threads:
#ifndef NUMTHREADS
#define NUMTHREADS		    2
#endif

// setting the number of subdivisions:
#ifndef NUMNODES
#define NUMNODES        1
#endif

// setting the number of trials:
#ifndef NUMTRIALS
#define NUMTRIALS	10
#endif

float
Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float r = 1. - xn - yn;
	if( r <= 0. )
	        return 0.;
	float height = pow( r, 1./(float)N );
	return height;
}

int main( int argc, char *argv[ ] )
{
	omp_set_num_threads(NUMTHREADS);

	// the area of a single full-sized tile:
	// (not all tiles are full-sized, though)

	float fullTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
				( ( YMAX - YMIN )/(float)(NUMNODES-1) )  );

	// sum up the weighted heights into the variable "volume"
	// using an OpenMP for loop and a reduction:
    float maxPerformance = 0.0;
    float maxVolume = 0.0;

    for(int i = 0; i < NUMTRIALS; i++)
    {
        // start of timer
        double timeBegin = omp_get_wtime();

        float volume = 0.0;

        #pragma omp parallel for default(none),shared(fullTileArea),reduction(+:volume)
        for( int i = 0; i < NUMNODES*NUMNODES; i++ )
        {
            int iu = i % NUMNODES;
            int iv = i / NUMNODES;
            float z = Height( iu, iv ) * 2;
            // No matter the number of nodes, there will always be only 4 quarter tiles
            // Every increment of nodes (starting at 2 nodes), increases the number of halves by 4
            
            if ( ((iv > 0 && iv < NUMNODES-1) && iu == 0) || (iv == NUMNODES-1 && (iu > 0 && iu < NUMNODES-1)) || (iv == 0 && (iu > 0 && iu < NUMNODES-1)) || ((iv > 0 && iv < NUMNODES-1) && iu == NUMNODES-1) )
            {
                volume += z * fullTileArea/2.0;
            }
            
            else if( (iv == 0 && iu == 0) || (iv == 0 && iu == NUMNODES-1) || (iv == NUMNODES-1 && iu == 0) || (iv == NUMNODES-1 && iu == NUMNODES-1) )
            {
                volume += z * fullTileArea/4.0;
            }

            else 
            {
                volume += z * fullTileArea;
            }
        }

        // end of timer
        double timeEnd = omp_get_wtime();

        // megaheights computed per second = number of loops/ time passed/ 1,000,000
        double megaHeightsComputedPerSecond = ((double) (NUMNODES * NUMNODES)) / (timeEnd - timeBegin) / 1000000;
        
        // If new performance calculation is greater thanthe previous max
        if (megaHeightsComputedPerSecond > maxPerformance) maxPerformance = megaHeightsComputedPerSecond;
        
        // If new volume is greater than the previous max volume
        if (volume > maxVolume) maxVolume = volume;
    }   
    // printf("NUMTHREADS: %d\t NUMNODES: %d\t MegaHeightComputedPerSecond: %10.2lf\t Volume: %10.2lf\t\n", NUMTHREADS, NUMNODES, maxPerformance, maxVolume);
    printf("%d, %d, %10.2lf, %10.2lf\n", NUMTHREADS, NUMNODES, maxPerformance, maxVolume);
}