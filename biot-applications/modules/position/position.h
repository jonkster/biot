#ifndef _POSITION_H_
#define _POSITION_H_

typedef struct {
    float w;
    float x;
    float y;
    float z;
} myQuat_t;

#ifdef __cplusplus
extern "C" {
#endif

    void dumpQuat(myQuat_t q);

    void dumpVec(double *v);

    void makeIdentityQuat(myQuat_t *q);

    myQuat_t makeQuatFromAngularVelocityTime(double *omega, double dt);

    myQuat_t newQuat(void);

    float quatLength(myQuat_t q);

    void quatNormalise(myQuat_t *q);

    myQuat_t quatMultiply(myQuat_t p, myQuat_t q);

    myQuat_t quatFrom2Vecs(double *u, double *v);

    myQuat_t slerp(myQuat_t qa, myQuat_t qb, double t);

    void vecCross(double *dest, double *u, double *v);

    double vecLength(double *v);

    double *vecNormalise(double *v);

#ifdef __cplusplus
}
#endif

#endif
