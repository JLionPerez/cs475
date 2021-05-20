#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

void	InitBarrier( int );
void	WaitBarrier( );
float Ranf( unsigned int *seedp,  float low, float high );
int Ranf( unsigned int *seedp, int ilow, int ihigh );
float SQR( float x );


omp_lock_t	Lock;
int		NumInThreadTeam;
int		NumAtBarrier;
int		NumGone;

#ifndef N
#define N 4
#endif

unsigned int seed = 0;
float x = Ranf( &seed, -1.f, 1.f );

int	NowYear;		// 2021 - 2026
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
int NowNumWormMonster; // number of worm monsters in the current population

const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;
const float ONE_WORM_EATS_PER_MONTH = 2.0;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

int main() {
	// starting date and time:
	NowMonth =    0;
	NowYear  = 2021;

	// starting state (feel free to change this if you want):
	NowNumDeer = 4;
	NowHeight =  1.;
	NowNumWormMonster = 1;

	float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

	float temp = AVG_TEMP - AMP_TEMP * cos( ang );
	NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
	if( NowPrecip < 0. )
		NowPrecip = 0.;
	printf("Months, Deer, Height (cm), Precip (cm), Temp (C), Worm Monsters\n");
	omp_set_num_threads( 4 );	// same as # of sections
	#pragma omp parallel sections
	{
		InitBarrier(4);
		#pragma omp section
		{
			// Deer( );
			while( NowYear < 2027 )
			{
				// compute a temporary next-value for this quantity
				// based on the current state of the simulation:
				int nextNumDeer = NowNumDeer;
				int carryingCapacity = (int)( NowHeight );

				if( nextNumDeer < carryingCapacity )
        			nextNumDeer++;

				else 
					if( nextNumDeer > carryingCapacity )
                		nextNumDeer--;

				nextNumDeer -= NowNumWormMonster * ONE_WORM_EATS_PER_MONTH;

				if( nextNumDeer < 0 )
       				nextNumDeer = 0;

				// DoneComputing barrier:
				// #pragma omp barrier
				WaitBarrier();
				NowNumDeer = nextNumDeer;

				// DoneAssigning barrier:
				// #pragma omp barrier
				WaitBarrier();

				// DonePrinting barrier:
				// #pragma omp barrier
				WaitBarrier();
			}
		}

		#pragma omp section
		{
			// Grain( );
			while( NowYear < 2027 )
			{
				// compute a temporary next-value for this quantity
				// based on the current state of the simulation:
				float nextHeight = NowHeight;

				float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
				float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

				nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
				nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

				if( nextHeight < 0. ) nextHeight = 0.;

				// DoneComputing barrier:
				WaitBarrier();
				NowHeight = nextHeight;

				// DoneAssigning barrier:
				WaitBarrier();

				// DonePrinting barrier:
				WaitBarrier();
			}
		}

		#pragma omp section
		{
			// Worm Monster( );	// your own
			while( NowYear < 2027 )
			{
				// compute a temporary next-value for this quantity
				// based on the current state of the simulation:
				int nextNumWormMonsters = NowNumWormMonster;
				if (NowTemp <= 45.0) {
					nextNumWormMonsters += 2;
				}
				else {
					nextNumWormMonsters -= 1;
				}

				if (nextNumWormMonsters < 0) {
					nextNumWormMonsters = 0;
				}

				// DoneComputing barrier:
				WaitBarrier();
				NowNumWormMonster = nextNumWormMonsters;

				// DoneAssigning barrier:
				WaitBarrier();
				
				// DonePrinting barrier:
				WaitBarrier();
			}
		}

		#pragma omp section
		{
			while( NowYear < 2027 )
			{
				// Watcher()
				// compute a temporary next-value for this quantity
				// based on the current state of the simulation:
				

				// DoneComputing barrier:
				WaitBarrier();
				

				// DoneAssigning barrier:
				WaitBarrier();
				
				float convertedHeight = NowHeight * 2.54;
				float convertedPrecip = NowPrecip * 2.54;
				float convertedTemp = (5.0 / 9.0) * (NowTemp - 32);

				printf("%d, %d, %lf, %lf, %lf, %d\n", NowMonth, NowNumDeer, convertedHeight, convertedPrecip, convertedTemp, NowNumWormMonster);

				// Increment time
				if (NowMonth == 11) {
					NowYear++;
					NowMonth = 0;
				} 
				else {
					NowMonth++;
				}

				ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

				temp = AVG_TEMP - AMP_TEMP * cos( ang );
				NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

				precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
				NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
				if( NowPrecip < 0. )
					NowPrecip = 0.;

				// DonePrinting barrier:
				WaitBarrier();
			}
		}

	}       // implied barrier -- all functions must return in order
		// to allow any of them to get past here
}


float
SQR( float x )
{
        return x*x;
}

// specify how many threads will be in the barrier:
//	(also init's the Lock)

void
InitBarrier( int n )
{
        NumInThreadTeam = n;
        NumAtBarrier = 0;
	omp_init_lock( &Lock );
}

// have the calling thread wait here until all the other threads catch up:

void
WaitBarrier( )
{
        omp_set_lock( &Lock );
        {
                NumAtBarrier++;
                if( NumAtBarrier == NumInThreadTeam )
                {
                        NumGone = 0;
                        NumAtBarrier = 0;
                        // let all other threads get back to what they were doing
			// before this one unlocks, knowing that they might immediately
			// call WaitBarrier( ) again:
                        while( NumGone != NumInThreadTeam-1 );
                        omp_unset_lock( &Lock );
                        return;
                }
        }
        omp_unset_lock( &Lock );

        while( NumAtBarrier != 0 );	// this waits for the nth thread to arrive

        #pragma omp atomic
        NumGone++;			// this flags how many threads have returned
}

float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int
Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}

