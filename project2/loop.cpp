#pragma omp parallel for default(none) . . .
for( int i = 0; i < NUMNODES*NUMNODES; i++ )
{
	int iu = i % NUMNODES;
	int iv = i / NUMNODES;
	float z = Height( iu, iv );
	//. . .
}

//. . .