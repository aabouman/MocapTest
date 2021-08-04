/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.5 */

#ifndef PB_MESSAGING_NODES_VICON_RELAY_REXLABVICON_PROTO_VICON_MSG_PB_H_INCLUDED
#define PB_MESSAGING_NODES_VICON_RELAY_REXLABVICON_PROTO_VICON_MSG_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _messaging_VICON { 
    double pos_x; 
    double pos_y; 
    double pos_z; 
    double quat_w; 
    double quat_x; 
    double quat_y; 
    double quat_z; 
    double time; 
} messaging_VICON;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define messaging_VICON_init_default             {0, 0, 0, 0, 0, 0, 0, 0}
#define messaging_VICON_init_zero                {0, 0, 0, 0, 0, 0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define messaging_VICON_pos_x_tag                1
#define messaging_VICON_pos_y_tag                2
#define messaging_VICON_pos_z_tag                3
#define messaging_VICON_quat_w_tag               4
#define messaging_VICON_quat_x_tag               5
#define messaging_VICON_quat_y_tag               6
#define messaging_VICON_quat_z_tag               7
#define messaging_VICON_time_tag                 8

/* Struct field encoding specification for nanopb */
#define messaging_VICON_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, DOUBLE,   pos_x,             1) \
X(a, STATIC,   SINGULAR, DOUBLE,   pos_y,             2) \
X(a, STATIC,   SINGULAR, DOUBLE,   pos_z,             3) \
X(a, STATIC,   SINGULAR, DOUBLE,   quat_w,            4) \
X(a, STATIC,   SINGULAR, DOUBLE,   quat_x,            5) \
X(a, STATIC,   SINGULAR, DOUBLE,   quat_y,            6) \
X(a, STATIC,   SINGULAR, DOUBLE,   quat_z,            7) \
X(a, STATIC,   SINGULAR, DOUBLE,   time,              8)
#define messaging_VICON_CALLBACK NULL
#define messaging_VICON_DEFAULT NULL

extern const pb_msgdesc_t messaging_VICON_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define messaging_VICON_fields &messaging_VICON_msg

/* Maximum encoded size of messages (where known) */
#define messaging_VICON_size                     72

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
