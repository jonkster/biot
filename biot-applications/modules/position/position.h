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

    void makeIdentityQuat(myQuat_t *q);

    void quatNormalise(myQuat_t *q);

    myQuat_t quatMultiply(myQuat_t p, myQuat_t q);

    myQuat_t quatFrom2Vecs(double *u, double *v);

#ifdef __cplusplus
}
#endif

#endif
