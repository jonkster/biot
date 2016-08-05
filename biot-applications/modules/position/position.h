#ifndef _POSITION_H_
#define _POSITION_H_

typedef struct {
    double w;
    double x;
    double y;
    double z;
} myQuat_t;


#ifdef __cplusplus
extern "C" {
#endif

    void dumpQuat(myQuat_t q);

    void dumpVec(double *v);

    double vecDot(double *u, double *v);

    void makeIdentityQuat(myQuat_t *q);

    myQuat_t deltaQuat(myQuat_t startQ, myQuat_t finalQ);

    myQuat_t makeQuatFromAngularVelocityTime(double *omega, double dt);

    myQuat_t newQuat(void);

    myQuat_t quatAngleAxis(double angleRad, double *axis);

    myQuat_t quatConjugate(myQuat_t q);

    myQuat_t quatFromValues(double w, double x, double y, double z);

    double quatLength(myQuat_t q);

    void quatNormalise(myQuat_t *q);

    myQuat_t quatMultiply(myQuat_t p, myQuat_t q);

    void quatMultiplyVec(double *destVec, myQuat_t q, double *v);

    myQuat_t quatFrom2Vecs(double *u, double *v);

    myQuat_t quatScalarMultiply(myQuat_t q, double s);

    double qAngle(myQuat_t q);

    myQuat_t slerp(myQuat_t qa, myQuat_t qb, double t);

    void vecCross(double *dest, double *u, double *v);

    double vecLength(double *v);

    double *vecNormalise(double *v);

    void vecScalarMultiply(double *dest, double s);

#ifdef __cplusplus
}
#endif

#endif
