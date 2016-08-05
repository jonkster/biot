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

void dumpVec(double *v)
{
    printf("v{%f, %f, %f}\n", v[0], v[1], v[2]);
}

double vecDot(double *u, double *v)
{
    double d = u[0]*v[0] + u[1]*v[1] + u[2]*v[2]; 
    return d; 
}

void vecCross(double *dest, double *u, double *v)
{
    dest[0] = u[1] * v[2] - u[2] * v[1];
    dest[1] = u[2] * v[0] - u[0] * v[2];
    dest[2] = u[0] * v[1] - u[1] * v[0];
}

double vecLength(double *v)
{
     return sqrt((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]));
}

double *vecNormalise(double *v)
{
    double length = vecLength(v);
    if (length > 0)
    {
        v[0] = v[0]/length;
        v[1] = v[1]/length;
        v[2] = v[2]/length;
    }
    return v;
}

void vecScalarMultiply(double *dest, double s)
{
    dest[0] *= s;
    dest[1] *= s;
    dest[2] *= s;
}

myQuat_t quatConjugate(myQuat_t q)
{
    return quatFromValues(q.w, -q.x, -q.y, -q.z);
}

/**
 * Calculate the quaternion that would move startQ to finalQ
 */
myQuat_t deltaQuat(myQuat_t startQ, myQuat_t finalQ)
{
    // deltaq = finalQ * startQ^-1
    myQuat_t invStart = quatConjugate(startQ);
    return quatMultiply(finalQ, invStart);
}

/**
 * Rotate vector by quaternion
 */
void quatMultiplyVec(double *destVec, myQuat_t q, double *v)
{
    myQuat_t qOfVec = quatFromValues(0, v[0], v[1], v[2]);
    myQuat_t qConj = quatConjugate(q);
    myQuat_t dest = quatMultiply(q, qOfVec);
    dest = quatMultiply(dest, qConj);
    destVec[0] = dest.x;
    destVec[1] = dest.y;
    destVec[2] = dest.z;
}

myQuat_t quatAngleAxis(double angleRad, double *axis)
{
    myQuat_t q = quatFromValues(
            cos(angleRad/2),
            axis[0] * sin(angleRad/2),
            axis[1] * sin(angleRad/2),
            axis[2] * sin(angleRad/2)
            );
    quatNormalise(&q);
    return q;
}

myQuat_t makeQuatFromAngularVelocityTime(double *omega, double dt)
{
    // we are rotating a number of degrees about an axis;
    double dToRad = PI/180.0;
    double d[3] = { dToRad*omega[0]*dt,  dToRad*omega[1]*dt,  dToRad*omega[2]*dt };
    double angle = vecLength(d);
    double *axis = vecNormalise(d);
    return quatAngleAxis(angle, axis);
}

myQuat_t newQuat(void)
{
    myQuat_t q;
    makeIdentityQuat(&q);
    return q;
}

myQuat_t quatFromValues(double w, double x, double y, double z)
{
    myQuat_t q;
    q.w = w;
    q.x = x;
    q.y = y;
    q.z = z;
    return q;
}


/*
 * return a quaternion representing the rotation from vector u to vector v
 */
/*myQuat_t quatFrom2Vecs(double *u, double *v)
{
    myQuat_t q;

    double w[3];
    vecCross(w, u, v);
    double ww = vecDot(u, v);
    if (ww < -0)
    {
        puts("flip?");
    }
    q.w = ww;
    q.x = w[0];
    q.y = w[1];
    q.z = w[2];
    q.w += quatLength(q);
    quatNormalise(&q);
    return q;
}*/

myQuat_t quatFrom2Vecs(double *u, double *v)
{
    // Based on Stan Melax's article in Game Programming Gems
    myQuat_t q;
    vecNormalise(u);
    vecNormalise(v);

    double d = vecDot(u, v);
    // If dot == 1, vectors are parallel
    if (d >= 1.0f)
    {
	makeIdentityQuat(&q);
	return q;
    }

    // If dot == -1 vectors are anti parallel, do a 180 about any axis
    if (d < (1e-6f - 1.0f))
    {
        // rotation 180d about... X axis?
        q = quatFromValues(0, 1, 0, 0);
    }
    else
    {
	double s = sqrt( (1+d)*2 );
	double invs = 1 / s;
	double c[3];
	vecCross(c, u, v);
	q.x = c[0] * invs;
	q.y = c[1] * invs;
	q.z = c[2] * invs;
	q.w = s * 0.5f;
    	quatNormalise(&q);
    }
    return q;
}

myQuat_t quatScalarMultiply(myQuat_t q, double s)
{
    q.w *= s;
    q.x *= s;
    q.y *= s;
    q.z *= s;
    return q;
}

void makeIdentityQuat(myQuat_t *q)
{
    q->w = 1;
    q->x = 0;
    q->y = 0;
    q->z = 0;
}

// magnitude of rotation
double qAngle(myQuat_t q)
{
    double vecPart[3] = { q.x, q.y, q.z };
    double angle =  2 * atan2(vecLength(vecPart), q.w);
    if (angle > PI)
        angle = fabs(2*PI - angle);
    return angle;
}


myQuat_t quatMultiply(myQuat_t q1, myQuat_t q2)
{
    myQuat_t dest;

    dest.x =  q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
    dest.y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
    dest.z =  q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
    dest.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;

    quatNormalise(&dest);
    return dest;
}

double quatLength(myQuat_t q)
{
    double w = q.w;
    double x = q.x;
    double y = q.y;
    double z = q.z;
    return sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

void quatNormalise(myQuat_t *q)
{
    double w = q->w;
    double x = q->x;
    double y = q->y;
    double z = q->z;

    double len = quatLength(*q);
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

myQuat_t slerp(myQuat_t q1, myQuat_t q2, double t)
{
    myQuat_t dest = newQuat();

    double cosHalfTheta = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
    // if q1=q2 or q1=-q2 then theta = 0 and we can return q1
    if (fabs(cosHalfTheta) >= 1.0)
    {
	dest.w = q1.w;
	dest.x = q1.x;
	dest.y = q1.y;
	dest.z = q1.z;
	return dest;
    }

    double halfTheta = acos(cosHalfTheta);
    double sinHalfTheta = sqrt(1.0 - cosHalfTheta*cosHalfTheta);
    // if theta = 180 degrees then result is not fully defined
    // we could rotate around any axis normal to q1 or q2
    if (fabs(sinHalfTheta) < 0.001)
    {
	dest.w = (q1.w * 0.5 + q2.w * 0.5);
	dest.x = (q1.x * 0.5 + q2.x * 0.5);
	dest.y = (q1.y * 0.5 + q2.y * 0.5);
	dest.z = (q1.z * 0.5 + q2.z * 0.5);
	return dest;
    }
    double ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
    double ratioB = sin(t * halfTheta) / sinHalfTheta; 
    dest.w = (q1.w * ratioA + q2.w * ratioB);
    dest.x = (q1.x * ratioA + q2.x * ratioB);
    dest.y = (q1.y * ratioA + q2.y * ratioB);
    dest.z = (q1.z * ratioA + q2.z * ratioB);
    return dest;
}

