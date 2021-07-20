/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.5 */

#ifndef PB_MESSAGING_IMU_MSG_PB_H_INCLUDED
#define PB_MESSAGING_IMU_MSG_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _messaging_IMU { 
    double acc_x; 
    double acc_y; 
    double acc_z; 
    double gyr_x; 
    double gyr_y; 
    double gyr_z; 
} messaging_IMU;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define messaging_IMU_init_default               {0, 0, 0, 0, 0, 0}
#define messaging_IMU_init_zero                  {0, 0, 0, 0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define messaging_IMU_acc_x_tag                  1
#define messaging_IMU_acc_y_tag                  2
#define messaging_IMU_acc_z_tag                  3
#define messaging_IMU_gyr_x_tag                  5
#define messaging_IMU_gyr_y_tag                  6
#define messaging_IMU_gyr_z_tag                  7

/* Struct field encoding specification for nanopb */
#define messaging_IMU_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, DOUBLE,   acc_x,             1) \
X(a, STATIC,   SINGULAR, DOUBLE,   acc_y,             2) \
X(a, STATIC,   SINGULAR, DOUBLE,   acc_z,             3) \
X(a, STATIC,   SINGULAR, DOUBLE,   gyr_x,             5) \
X(a, STATIC,   SINGULAR, DOUBLE,   gyr_y,             6) \
X(a, STATIC,   SINGULAR, DOUBLE,   gyr_z,             7)
#define messaging_IMU_CALLBACK NULL
#define messaging_IMU_DEFAULT NULL

extern const pb_msgdesc_t messaging_IMU_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define messaging_IMU_fields &messaging_IMU_msg

/* Maximum encoded size of messages (where known) */
#define messaging_IMU_size                       54

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
