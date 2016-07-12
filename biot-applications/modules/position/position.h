#ifndef _POSITION_H_
#define _POSITION_H_

typedef struct {
    float w;
    float x;
    float y;
    float z;
} quat_t;

#ifdef __cplusplus
extern "C" {
#endif

    void dumpQuat(quat_t q);

    void makeIdentityQuat(quat_t *q);

    void quatNormalise(quat_t *q);

    quat_t quatMultiply(quat_t p, quat_t q);
    
#ifdef __cplusplus
}
#endif

#endif
