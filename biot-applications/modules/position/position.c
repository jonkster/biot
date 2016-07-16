#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>

#include "position.h"

void dumpQuat(myQuat_t q)
{
    printf("q{%f, %f, %f, %f}\n", q.w, q.x, q.y, q.z);
}

double vecDot(double *u, double *v)
{
    double d = u[0]*v[0] + u[1]+u[1] + u[2]*v[2]; 
    return d; 
}

double *vecCross(double *u, double *v)
{
    double *dest = (double*)malloc(3 * sizeof(double));
    dest[0] = u[1] * v[2] - u[2] * v[1];
    dest[1] = u[2] * v[0] - u[0] * v[2];
    dest[2] = u[0] * v[1] - u[1] * v[0];
    return dest;
}

myQuat_t quatFrom2Vecs(double *u, double *v)
{
    double normUV = sqrt(abs(vecDot(u, u) * vecDot(v, v)));
    double realPart = normUV + vecDot(u, v);
    double vec[3];

    if (realPart < 1.e-6f * normUV)
    {
	/* If u and v are exactly opposite, rotate 180 degrees
	 * around an arbitrary orthogonal axis. Axis normalisation
	 * can happen later, when we normalise the quaternion. */
	realPart = 0.0f;
	if (abs(u[0]) > abs(u[2]))
	{
	    vec[0] = -u[1];
	    vec[1] = u[0];
	    vec[2] = 0.f;
	}
	else
	{
	    vec[0] = 0.f;
	    vec[1] = u[2];
	    vec[2] = u[1];
	} 
    }
    else
    {
	/* Otherwise, build quaternion the standard way. */
	double *vTemp = vecCross(u, v);
        vec[0] = vTemp[0];
        vec[1] = vTemp[1];
        vec[2] = vTemp[2];
        free(vTemp);
    }
    myQuat_t q;
    q.w = realPart;
    q.x = vec[0];
    q.y = vec[1];
    q.z = vec[2];
    quatNormalise(&q);
    //dumpQuat(q);
    return q;
}

void makeIdentityQuat(myQuat_t *q)
{
    q->w = 1;
    q->x = 0;
    q->y = 0;
    q->z = 0;
}


myQuat_t quatMultiply(myQuat_t p, myQuat_t q)
{
    myQuat_t dest;

    dest.w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
    dest.x = p.x * q.w + p.w * q.x + p.y * q.z - p.z * q.y;
    dest.y = p.y * q.w + p.w * q.y + p.z * q.x - p.x * q.z;
    dest.z = p.z * q.w + p.w * q.z + p.x * q.y - p.y * q.x;

    p.w = dest.w;
    p.x = dest.x;
    p.y = dest.y;
    p.z = dest.z;
    return p;
}

void quatNormalise(myQuat_t *q)
{
    float w = q->w;
    float x = q->x;
    float y = q->y;
    float z = q->z;

    float len = sqrt(x * x + y * y + z * z + w * w);
    if (len == 0) {
        q->w = 0;
        q->x = 0;
        q->y = 0;
        q->z = 0;
    }
    else
    {
        len = 1 / len;
        q->w = w * len;
        q->x = x * len;
        q->y = y * len;
        q->z = z * len;
    }
}

