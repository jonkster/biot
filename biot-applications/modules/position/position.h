#ifndef _POSITION_H_
#define _POSITION_H_

typedef struct {
    double w;
    double x;
    double y;
    double z;
} myQuat_t;

#define YAW 0
#define PITCH 1
#define ROLL 2

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

#ifdef __cplusplus
extern "C" {
#endif

    myQuat_t deltaQuat(myQuat_t startQ, myQuat_t finalQ);

    void dumpQuat(myQuat_t q);

    void dumpVec(double *v);

    myQuat_t eulerToQuat(double *rpy);

    bool isQuatValid(myQuat_t q);

    void makeIdentityQuat(myQuat_t *q);

    myQuat_t makeQuatFromAngularVelocityTime(double *omega, double dt);

    myQuat_t newQuat(void);

    double qAngle(myQuat_t q);

    double quatDiffMagnitude(myQuat_t a, myQuat_t b);

    myQuat_t quatAngleAxis(double angleRad, double *axis);

    myQuat_t quatConjugate(myQuat_t q);

    myQuat_t quatFrom2Vecs(double *u, double *v);

    myQuat_t quatFromValues(double w, double x, double y, double z);

    double quatLength(myQuat_t q);

    myQuat_t quatMultiply(myQuat_t p, myQuat_t q);

    void quatMultiplyVec(double *destVec, myQuat_t q, double *v);

    void quatNormalise(myQuat_t *q);

    myQuat_t quatScalarMultiply(myQuat_t q, double s);

    void quatToEuler(myQuat_t q, double *rpy);

    myQuat_t slerp(myQuat_t qa, myQuat_t qb, double t);

    double vecDot(double *u, double *v);

    double vecLength(double *v);

    double *vecNormalise(double *v);

    void vecCross(double *dest, double *u, double *v);

    void vecScalarMultiply(double *dest, double s);

#ifdef __cplusplus
}
#endif

#endif
