// #include "simd.h"

// static void do_rotation( const Protein & x,  Protein & y,
//                       int len, float u[3][3]) {
//     //len: length of atoms   int
//     //u: rotation matrix     3x3

//     simd_float u00 = simdf32_set(u[0][0]);
//     simd_float u01 = simdf32_set(u[0][1]);
//     simd_float u02 = simdf32_set(u[0][2]);
//     simd_float u10 = simdf32_set(u[1][0]);
//     simd_float u11 = simdf32_set(u[1][1]);
//     simd_float u12 = simdf32_set(u[1][2]);
//     simd_float u20 = simdf32_set(u[2][0]);
//     simd_float u21 = simdf32_set(u[2][1]);
//     simd_float u22 = simdf32_set(u[2][2]);
//     for(int i=0; i<len; i+=VECSIZE_FLOAT) {
//         simd_float x_x = simdf32_load(&x.x[i]);
//         simd_float x_y = simdf32_load(&x.y[i]);
//         simd_float x_z = simdf32_load(&x.z[i]);
//         simd_float xx = simdf32_mul(u00, x_x);
//         simd_float yy = simdf32_mul(u01, x_y);
//         simd_float zz = simdf32_mul(u02, x_z);
//         xx = simdf32_add(xx, yy);
//         simdf32_store(&y.x[i], simdf32_add(xx, zz));
//         xx = simdf32_mul(u10, x_x);
//         yy = simdf32_mul(u11, x_y);
//         zz = simdf32_mul(u12, x_z);
//         xx = simdf32_add(xx, yy);
//         simdf32_store(&y.y[i],simdf32_add(xx, zz));
//         xx = simdf32_mul(u20, x_x);
//         yy = simdf32_mul(u21, x_y);
//         zz = simdf32_mul(u22, x_z);
//         xx = simdf32_add(xx, yy);
//         simdf32_store(&y.z[i], simdf32_add(xx, zz));
//     }
// }

// static void do_translation(const Protein & x,  Protein & y,
//                       int len, float t[3]) {
//     //t: translation matrix  3x1
//     simd_float t0 = simdf32_set(t[0]);
//     simd_float t1 = simdf32_set(t[1]);
//     simd_float t2 = simdf32_set(t[2]);
//     for(int i=0; i<len; i+=VECSIZE_FLOAT) {
//         simd_float x_x = simdf32_load(&x.x[i]);
//         simd_float x_y = simdf32_load(&x.y[i]);
//         simd_float x_z = simdf32_load(&x.z[i]);
//         simdf32_store(&y.x[i], simdf32_add(t0, x_x));
//         simdf32_store(&y.y[i], simdf32_add(t1, x_y));
//         simdf32_store(&y.z[i], simdf32_add(t2, x_z));
//     }
// }