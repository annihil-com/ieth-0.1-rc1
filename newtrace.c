typedef struct {
	vec3_t pt;
	float  d;
} trace_point;

int distance_test(const void *elm1, const void *elm2)
{
	const trace_point *a = elm1;
	const trace_point *b = elm2;
		
	if( a->d > b->d )
		return 1;
	else if( a->d < b->d )
		return -1;
	else
		return 0;
}


/* 	multi trace a box and return visible point
	boxes have an origin, and are always world-axis aligned
	Note: box is assumed to be centered on its origin
*/
qboolean trace_box( vec3_t box_origin, vec3_t size, vec3_t tr_origin, int skipent, qboolean random, qboolean multi,  vec3_t visible )
{
#define MAX_TR	16
	trace_t 	tr;
	trace_point p[MAX_TR];
	vec3_t 		x,y,z;

	//trap_CM_BoxTrace( &tr, tr_origin, box_origin, NULL, NULL, 0, MASK_SHOT );
	/* center trace first */
	eth.CG_Trace(&tr, tr_origin, NULL,NULL, box_origin, skipent, MASK_SHOT );
	/* calling trap_trace() is a lot faster, but it doesn't check for clips by movers :( */
	//trap_CM_BoxTrace( &tr, tr_origin, box_origin, NULL, NULL, 0, MASK_SHOT );
	if( tr.fraction == 1.f ){
		VectorCopy( tr.endpos, visible );
		return qtrue;
	} else if( (!multi && !random) || VectorCompare(size,vec3_origin) )
		return qfalse;

	int k = 0;
	memset(p, 0, sizeof(p));

	/* Y-axis points north */
	x[0] = 1.0f; x[1] = 0.0f; x[2] = 0.0f;
	y[0] = 0.0f; y[1] = 1.0f; y[2] = 0.0f;
	z[0] = 0.0f; z[1] = 0.0f; z[2] = 1.0f;
	
	if( !random ){
		/* trace periphery - prioritize plane centers, more chance of hitting */
		VectorMA( box_origin, size[0]*0.5, x, p[k].pt ); k++;
		VectorMA( box_origin, -size[0]*0.5, x, p[k].pt ); k++;
		VectorMA( box_origin, size[1]*0.5, y, p[k].pt ); k++;
		VectorMA( box_origin, -size[0]*0.5, y, p[k].pt ); k++;
		VectorMA( box_origin, size[2]*0.5, z, p[k].pt ); k++;

		/* corners */
		VectorMA( box_origin, size[0]*0.5, x, p[k].pt );
		VectorMA( p[k].pt, size[2]*0.5, z, p[k].pt );
		k++;

		VectorMA( box_origin, -size[0]*0.5, x, p[k].pt );
		VectorMA( p[k].pt, size[2]*0.5, z, p[k].pt );
		k++;

		VectorMA( box_origin, size[1]*0.5, y, p[k].pt );
		VectorMA( p[k].pt, size[2]*0.5, z, p[k].pt );
		k++;

		VectorMA( box_origin, -size[1]*0.5, y, p[k].pt );
		VectorMA( p[k].pt, size[2]*0.5, z, p[k].pt );
		k++;
	} else {

	/* trace random points within hbox volume */
		for( k=0; k < MAX_TR; k++ ){
			while( p[k].d < 0.25 ){
				VectorMA( p[k].pt, size[0]*crandom()*0.5, x, p[k].pt );
				VectorMA( p[k].pt, size[1]*crandom()*0.5, y, p[k].pt );
				VectorMA( p[k].pt, size[2]*crandom()*0.5, z, p[k].pt );
				p[k].d = VectorLengthSquared( p[k].pt );
			}
		}
		qsort( p, MAX_TR, sizeof(trace_point), distance_test );

		for( k=0; k < MAX_TR; k++ )
			VectorAdd( box_origin, p[k].pt, p[k].pt );
	}


	/* trace them all */
	for( k=0; k<MAX_TR; k++ ){
		if( VectorCompare(p[k].pt, vec3_origin) )
			continue;
		memset( &tr, 0, sizeof(tr) );
		eth.CG_Trace(&tr, tr_origin, NULL,NULL, p[k].pt, skipent, MASK_SHOT );
	//trap_CM_BoxTrace( &tr, tr_origin, p[k], NULL, NULL, 0, MASK_SHOT );
		if( tr.fraction == 1.f ){
			VectorCopy( tr.endpos, visible );
			return qtrue;
		}
	}

	return qfalse;
}