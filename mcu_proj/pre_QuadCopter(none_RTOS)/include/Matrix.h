/*
 * Matrix.h
 *
 *  Created on: 2018. 8. 12.
 *      Author: HyunwooPark
 */

#ifndef INCLUDE_MATRIX_H_
#define INCLUDE_MATRIX_H_

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
typedef struct matrix_3d max3;
typedef struct matrix_1d max1;

struct matrix_3d{ // 3x3 row1(x0,x1,x2) , row2(y0,y1,y2), row3(z0,z1,z2)

    float x[3];
    float y[3];
    float z[3];

    void (* print)(max3);
    void (* add)(max3, max3, max3 *);
    void (* sub)(max3, max3, max3 *);
    void (* mul)(float, max3, max3 *);
    void (* mul_3_3)(max3, max3, max3 *);
    float (* det)(max3);

    void (* inverse)(max3, max3 *);
    void (* crammer)(max3, max3, max3 *);
};

struct matrix_1d{  // 3x1

    float x,y,z;

    void (* print)(max1);
    void (* mul_3_1)(max3, max1, max1 *); // 3 x 3 * 3 x 1
};

void max1_print(max1 a){
//    int i;
//    printf("\t%lf\n\t%lf\n\t%lf\n", a.x, a.y, a.z);
}

void max1_3by3_x_3by1(max3 a, max1 b, max1 *r){


    r->x = a.x[0]*b.x + a.x[1]*b.y + a.x[2]*b.z;
    r->y = a.y[0]*b.x + a.y[1]*b.y + a.y[2]*b.z;
    r->z = a.z[0]*b.x + a.z[1]*b.y + a.z[2]*b.z;
}

void max3_print(max3 a){
    int i;
    for(i=0;i<3; i++){
        printf("\t%lf\t%lf\t%lf\n", a.x[i], a.y[i], a.z[i]);
    }
}


void max3_add(max3 a, max3 b, max3 *r){

    int i;

    for( i=0; i<3; i++){
        r->x[i] = a.x[i] + b.x[i];
        r->y[i] = a.y[i] + b.y[i];
        r->z[i] = a.z[i] + b.z[i];
    }
}

void max3_sub(max3 a, max3 b, max3 *r){
    int i;
    for( i=0; i<3;i++){
        r->x[i] = a.x[i] - b.x[i];
        r->y[i] = a.y[i] - b.y[i];
        r->z[i] = a.z[i] - b.z[i];
    }
}

void max3_mul(float num, max3 a, max3 *r){
    int i;
    for( i=0; i<3;i++){
        r->x[i] = num * a.x[i];
        r->y[i] = num * a.y[i];
        r->z[i] = num * a.z[i];
    }

}

void max3_3_by_3_mul(max3 a, max3 b, max3 *r){

    int i;

    for(i =0;i<3;i++){
        r->x[i] = a.x[0] * b.x[i] + a.x[1] * b.y[i] + a.x[2] * b.z[i];
        r->y[i] = a.y[0] * b.x[i] + a.y[1] * b.y[i] + a.y[2] * b.z[i];
        r->z[i] = a.z[0] * b.x[i] + a.z[1] * b.y[i] + a.z[2] * b.z[i];
    }
}

float max3_det(max3 a){

    float w0 = a.x[0] * ( a.y[1] *a.z[2] - a.z[1] * a.y[2]);
    float w1 = -a.y[0] * ( a.x[1] *a.z[2] - a.z[1] * a.x[2]);
    float w2 = a.z[0] * ( a.x[1] *a.y[2] - a.y[1] * a.x[2]);

    float det = w0 + w1 + w2;
    return det;
}

void max3_inverse(max3 a, max3 *r){

    float det = max3_det(a);
    max3 adj;
    max3 t_pose;

    if(det != 0){
        adj.x[0] = (a.y[1]*a.z[2]) - (a.z[1] * a.y[2]);
        adj.x[1] = -(a.x[1]*a.z[2]) + (a.z[1] * a.x[2]);
        adj.x[2] = (a.x[1]*a.y[2]) - (a.y[1] * a.x[2]);

        adj.y[0] = -(a.y[0]*a.z[2]) + (a.z[0] * a.y[2]);
        adj.y[1] =  (a.x[0]*a.z[2]) - (a.z[0] * a.x[2]);
        adj.y[2] = -(a.x[0]*a.y[2]) + (a.y[0] * a.x[2]);

        adj.z[0] = (a.y[0]*a.z[1]) - (a.z[0] * a.y[1]);
        adj.z[1] = -(a.x[0]*a.z[1]) + (a.z[0] * a.x[1]);
        adj.z[2] = (a.x[0]*a.y[1]) - (a.y[0] * a.x[1]);

//        printf("ADJ\n");
//        max3_print(adj);

        t_pose.x[0] = adj.x[0];
        t_pose.x[1] = adj.y[0];
        t_pose.x[2] = adj.z[0];

        t_pose.y[0] = adj.x[1];
        t_pose.y[1] = adj.y[1];
        t_pose.y[2] = adj.z[1];

        t_pose.z[0] = adj.x[2];
        t_pose.z[1] = adj.y[2];
        t_pose.z[2] = adj.z[2];

//        printf("TRANSPOSE\n");
//        max3_print(t_pose);
//        printf("INVERSE\n");
        max3_mul( 1.0/det, t_pose, r);
    }else{
//        printf("det is 0\n");
    }
}

bool invertColumnMajor(float m[16], float invOut[16])  // 4by4 inverse matrix
{
    float inv[16], det;
    int i;

    inv[ 0] =  m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
    inv[ 4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
    inv[ 8] =  m[4] * m[ 9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[ 9];
    inv[12] = -m[4] * m[ 9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[ 9];
    inv[ 1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
    inv[ 5] =  m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
    inv[ 9] = -m[0] * m[ 9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[ 9];
    inv[13] =  m[0] * m[ 9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[ 9];
    inv[ 2] =  m[1] * m[ 6] * m[15] - m[1] * m[ 7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[ 7] - m[13] * m[3] * m[ 6];
    inv[ 6] = -m[0] * m[ 6] * m[15] + m[0] * m[ 7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[ 7] + m[12] * m[3] * m[ 6];
    inv[10] =  m[0] * m[ 5] * m[15] - m[0] * m[ 7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[ 7] - m[12] * m[3] * m[ 5];
    inv[14] = -m[0] * m[ 5] * m[14] + m[0] * m[ 6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[ 6] + m[12] * m[2] * m[ 5];
    inv[ 3] = -m[1] * m[ 6] * m[11] + m[1] * m[ 7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[ 9] * m[2] * m[ 7] + m[ 9] * m[3] * m[ 6];
    inv[ 7] =  m[0] * m[ 6] * m[11] - m[0] * m[ 7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[ 8] * m[2] * m[ 7] - m[ 8] * m[3] * m[ 6];
    inv[11] = -m[0] * m[ 5] * m[11] + m[0] * m[ 7] * m[ 9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[ 9] - m[ 8] * m[1] * m[ 7] + m[ 8] * m[3] * m[ 5];
    inv[15] =  m[0] * m[ 5] * m[10] - m[0] * m[ 6] * m[ 9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[ 9] + m[ 8] * m[1] * m[ 6] - m[ 8] * m[2] * m[ 5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if(det == 0)
        return false;

    det = 1.f / det;

    for(i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}


void print_max4(float m[16]){

    int i;

    for(i=0; i<16; i++){
        if(i%4==0){
            printf("\n");
        }
            printf("\t%lf",m[i]);
    }
            printf("\n");

}

void m_4x4_4x1(float m[16], float a[4], float r[4]){   // mul 4by4 x 4by1

    r[0] = m[0] * a[0] + m[1] * a[1] + m[2] * a[2] + m[3] * a[3];
    r[1] = m[4] * a[0] + m[5] * a[1] + m[6] * a[2] + m[7] * a[3];
    r[2] = m[8] * a[0] + m[9] * a[1] + m[10] * a[2] + m[11] * a[3];
    r[3] = m[12] * a[0] + m[13] * a[1] + m[14] * a[2] + m[15] * a[3];

}
void m_4x1_print(float m[4]){

    printf("\t%lf\n\t%lf\n\t%lf\n\t%lf\n", m[0], m[1], m[2], m[3]);
}




#endif /* INCLUDE_MATRIX_H_ */
