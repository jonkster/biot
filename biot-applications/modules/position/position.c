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
     return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

double *vecNormalise(double *v)
{
    double length = vecLength(v);
    v[0] = v[0]/length;
    v[1] = v[1]/length;
    v[2] = v[2]/length;
    return v;
}

myQuat_t quatAngleAxis(double angleDeg, double *axis)
{
    double angle = angleDeg * PI/180;

    myQuat_t q;
    q.w = cos(angle/2);
    q.x = axis[0] * sin(angle/2);
    q.y = axis[1] * sin(angle/2);
    q.z = axis[2] * sin(angle/2);
    return q;
}

myQuat_t makeQuatFromAngularVelocityTime(double *omega, double dt)
{
    double d[3] = { omega[0]*dt,  omega[1]*dt,  omega[2]*dt };
    double angle = vecLength(d);
    myQuat_t q = newQuat();
    if (angle != 0)
    {
        q.x = omega[0] * sin(angle/2)/angle;
        q.y = omega[1] * sin(angle/2)/angle;
        q.z = omega[2] * sin(angle/2)/angle;
        q.w = cos(angle/2);
    }
    return q;
}

myQuat_t newQuat(void)
{
    myQuat_t q;
    makeIdentityQuat(&q);
    return q;
}

myQuat_t oldquatFrom2Vecs(double *u, double *v)
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
	double vTemp[3];
	vecCross(vTemp, u, v);
        vec[0] = vTemp[0];
        vec[1] = vTemp[1];
        vec[2] = vTemp[2];
    }
    myQuat_t q;
    q.w = realPart;
    q.x = vec[0];
    q.y = vec[1];
    q.z = vec[2];
    quatNormalise(&q);
    return q;
}

/*
 * return a quaternion representing the rotation from vector u to vector v
 */
myQuat_t quatFrom2Vecs(double *u, double *v)
{
    vecNormalise(u);
    vecNormalise(v);

    double cosTheta = vecDot(u, v);
    double rotationAxis[3];

    if (cosTheta < -1.0 + 1.0e-6 ) // vectors are anti-parallel
    {
        // we have 180d rotation, there is no "ideal" rotation axis, any axis 90
        // deg to the vectors
	double up[3] = { 0, 0, 1 };
	vecCross(rotationAxis, up, u);
	if (vecLength(rotationAxis) < 0.000001 ) // bad luck, up is parallel to our vector!
	{
	    double north[3] = { 1, 0, 0 };
	    vecCross(rotationAxis, north, u);
	}
	vecNormalise(rotationAxis);
	return quatAngleAxis(180.0, rotationAxis);
    }
    else if (cosTheta > 1.0 - 1.0e-6 ) // vectors are parallel
    {
        // there is no rotation! return an identity quaternion
        return newQuat();
    }

    vecCross(rotationAxis, u, v);

    double s = sqrt( (1+cosTheta)*2 );
    float invs = 1 / s;

    myQuat_t q;
    q.w = s * 0.5;
    q.x = rotationAxis[0] * invs;
    q.y = rotationAxis[1] * invs;
    q.z = rotationAxis[2] * invs;
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
    quatNormalise(&p);
    return p;
}

float quatLength(myQuat_t q)
{
    float w = q.w;
    float x = q.x;
    float y = q.y;
    float z = q.z;
    return sqrt(x * x + y * y + z * z + w * w);
}

void quatNormalise(myQuat_t *q)
{
    float w = q->w;
    float x = q->x;
    float y = q->y;
    float z = q->z;

    float len = quatLength(*q);
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

myQuat_t slerp(myQuat_t qa, myQuat_t qb, double t)
{
    myQuat_t dest = newQuat();

    double cosHalfTheta = qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z;
    // if qa=qb or qa=-qb then theta = 0 and we can return qa
    if (abs(cosHalfTheta) >= 1.0)
    {
	dest.w = qa.w;
	dest.x = qa.x;
	dest.y = qa.y;
	dest.z = qa.z;
	return dest;
    }

    double halfTheta = acos(cosHalfTheta);
    double sinHalfTheta = sqrt(1.0 - cosHalfTheta*cosHalfTheta);
    // if theta = 180 degrees then result is not fully defined
    // we could rotate around any axis normal to qa or qb
    if (fabs(sinHalfTheta) < 0.001)
    {
	dest.w = (qa.w * 0.5 + qb.w * 0.5);
	dest.x = (qa.x * 0.5 + qb.x * 0.5);
	dest.y = (qa.y * 0.5 + qb.y * 0.5);
	dest.z = (qa.z * 0.5 + qb.z * 0.5);
	return dest;
    }
    double ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
    double ratioB = sin(t * halfTheta) / sinHalfTheta; 
    dest.w = (qa.w * ratioA + qb.w * ratioB);
    dest.x = (qa.x * ratioA + qb.x * ratioB);
    dest.y = (qa.y * ratioA + qb.y * ratioB);
    dest.z = (qa.z * ratioA + qb.z * ratioB);
    return dest;
}

