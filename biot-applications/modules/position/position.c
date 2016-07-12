#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>

#include "position.h"

void dumpQuat(quat_t q)
{
    printf("q{%f, %f, %f, %f}\n", q.w, q.x, q.y, q.z);
}

void makeIdentityQuat(quat_t *q)
{
    q->w = 1;
    q->x = 0;
    q->y = 0;
    q->z = 0;
}


quat_t quatMultiply(quat_t p, quat_t q)
{
    quat_t dest;

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

void quatNormalise(quat_t *q)
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

