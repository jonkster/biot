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
    printf("v{%f, %f, %f}\n", v[X_AXIS], v[Y_AXIS], v[Z_AXIS]);
}

double vecDot(double *u, double *v)
{
    double d = u[X_AXIS]*v[X_AXIS] + u[Y_AXIS]*v[Y_AXIS] + u[Z_AXIS]*v[Z_AXIS]; 
    return d; 
}

void vecCross(double *dest, double *u, double *v)
{
    dest[X_AXIS] = u[Y_AXIS] * v[Z_AXIS] - u[Z_AXIS] * v[Y_AXIS];
    dest[Y_AXIS] = u[Z_AXIS] * v[X_AXIS] - u[X_AXIS] * v[Z_AXIS];
    dest[Z_AXIS] = u[X_AXIS] * v[Y_AXIS] - u[Y_AXIS] * v[X_AXIS];
}

double vecLength(double *v)
{
     return sqrt((v[X_AXIS]*v[X_AXIS]) + (v[Y_AXIS]*v[Y_AXIS]) + (v[Z_AXIS]*v[Z_AXIS]));
}

double *vecNormalise(double *v)
{
    double length = vecLength(v);
    if (length > 0)
    {
        v[X_AXIS] = v[X_AXIS]/length;
        v[Y_AXIS] = v[Y_AXIS]/length;
        v[Z_AXIS] = v[Z_AXIS]/length;
    }
    return v;
}

void vecScalarMultiply(double *dest, double s)
{
    dest[X_AXIS] *= s;
    dest[Y_AXIS] *= s;
    dest[Z_AXIS] *= s;
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
    myQuat_t qOfVec = quatFromValues(0, v[X_AXIS], v[Y_AXIS], v[Z_AXIS]);
    myQuat_t qConj = quatConjugate(q);
    myQuat_t dest = quatMultiply(q, qOfVec);
    dest = quatMultiply(dest, qConj);
    destVec[X_AXIS] = dest.x;
    destVec[Y_AXIS] = dest.y;
    destVec[Z_AXIS] = dest.z;
}

myQuat_t quatAngleAxis(double angleRad, double *axis)
{
    myQuat_t q = quatFromValues(
            cos(angleRad/2),
            axis[X_AXIS] * sin(angleRad/2),
            axis[Y_AXIS] * sin(angleRad/2),
            axis[Z_AXIS] * sin(angleRad/2)
            );
    quatNormalise(&q);
    return q;
}

myQuat_t makeQuatFromAngularVelocityTime(double *omega, double dt)
{
    // we are rotating a number of degrees about an axis;
    double dToRad = PI/180.0;
    double d[3] = { dToRad*omega[X_AXIS]*dt,  dToRad*omega[Y_AXIS]*dt,  dToRad*omega[Z_AXIS]*dt };
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
    q.x = w[X_AXIS];
    q.y = w[Y_AXIS];
    q.z = w[Z_AXIS];
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
	q.x = c[X_AXIS] * invs;
	q.y = c[Y_AXIS] * invs;
	q.z = c[Z_AXIS] * invs;
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

// magnitude of difference between two orientations
double quatDiffMagnitude(myQuat_t a, myQuat_t b)
{
    myQuat_t dq = deltaQuat(a, b);
    return qAngle(dq);

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

bool isQuatValid(myQuat_t q)
{
    return (! isnan(q.w));
}

myQuat_t eulerToQuat(double *ypr)
{
    /*
     * Order is YAW, PITCH then ROLL
     * roll = φ, pitch = θ, yaw = ψ.
     * q0 = cos(φ/2)*cos(θ/2)*cos(ψ/2)+sin(φ/2)*sin(θ/2)*sin(ψ/2)
     * q1 = sin(φ/2)*cos(θ/2)*cos(ψ/2)-cos(φ/2)*sin(θ/2)*sin(ψ/2)
     * q2 = cos(φ/2)*sin(θ/2)*cos(ψ/2)+sin(φ/2)*cos(θ/2)*sin(ψ/2)
     * q3 = cos(φ/2)*cos(θ/2)*sin(ψ/2)-sin(φ/2)*sin(θ/2)*cos(ψ/2)
     */

    // do divisions by 2 once
    double cosYawOver2 = cos(ypr[YAW] / 2.0);
    double sinYawOver2 = sin(ypr[YAW] / 2.0);
 
    double cosPitchOver2 = cos(ypr[PITCH] / 2.0);
    double sinPitchOver2 = sin(ypr[PITCH] / 2.0);

    double cosRollOver2 = cos(ypr[ROLL] / 2.0);
    double sinRollOver2 = sin(ypr[ROLL] / 2.0);

    double w = cosRollOver2 * cosPitchOver2 * cosYawOver2 + sinRollOver2 * sinPitchOver2 * sinYawOver2;
    double x = sinRollOver2 * cosPitchOver2 * cosYawOver2 - cosRollOver2 * sinPitchOver2 * sinYawOver2;
    double y = cosRollOver2 * sinPitchOver2 * cosYawOver2 + sinRollOver2 * cosPitchOver2 * sinYawOver2;
    double z = cosRollOver2 * cosPitchOver2 * sinYawOver2 - sinRollOver2 * sinPitchOver2 * cosYawOver2;
    myQuat_t q = quatFromValues(w, x, y, z);
    quatNormalise(&q);
    return q;
}

void quatToEuler(myQuat_t q, double *ypr)
{
    quatNormalise(&q);
    /* gimble lock will be an issue if pointing straight up or straight down...  */
    double pitchComp = q.w * q.y - q.z * q.x;
    if (pitchComp > 0.499)
    {
        ypr[YAW] = 2 * atan2(q.x, q.w);
        ypr[PITCH] = PI/2;
        ypr[ROLL] = 0;
    }
    else if (pitchComp < -0.499)
    {
        ypr[YAW] = -2 * atan2(q.x, q.w);
        ypr[PITCH] = PI/2;
        ypr[ROLL] = 0;
    }
    else
    {
        double ysqrd = q.y * q.y;
        ypr[ROLL] = (atan2(2.0 * (q.y * q.z + q.w * q.x), 1 - 2.0 * (q.x * q.x + ysqrd)));
        ypr[PITCH] = (asin(2.0 * (q.w * q.y - q.z * q.x)));
        ypr[YAW] = (atan2(2.0 * (q.x * q.y + q.w * q.z), 1 - 2.0 * (ysqrd + q.z * q.z)));
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

