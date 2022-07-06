#pragma once

#include "vector_stack.h"

#define MAX_IMU_HISTORY 20

typedef struct {
  int delay_comp;
  double* Wf;    // (6,) diagonal of mocap (measurement) noise
  double* Vf;    // (15,) diagonal of imu (process) noise
  double* xf;    // (16,) filter state [r,phi,v,vb,wb]
  double* Pf;    // (15,15) filter state covariance
  double* xd;    // (16,) delayed filter state [r,phi,v,vb,wb]
  double* Pd;    // (15,15) delayed filter state covariance
  double* xp;    // (16,) predicted filter state [r,phi,v,vb,wb]
  double* Pp;    // (15,15) predicted filter state covariance
  double* xn;    // (16,) updated filter state [r,phi,v,vb,wb]
  double* Pn;    // (15,15) updated filter state covariance
  double* xhat;  // (13,) state estimate
  rexquad_VectorQueue imuhist;

} rexquad_DelayedMEKF;

rexquad_DelayedMEKF rexquad_NewDelayedMEKF(int delay_comp);

void rexquad_FreeDelayedMEKF(rexquad_DelayedMEKF* filter);

void rexquad_InitializeDelayedMEKF(rexquad_DelayedMEKF* filter, int delay_comp,
                                   const double* x0, const double* Wf, const double* Vf,
                                   const double* b0, const double* Pf0);

void rexquad_SetInitialState(rexquad_DelayedMEKF* filter, const double* x0);

void rexquad_InitializeDelayMEKFDefault(rexquad_DelayedMEKF* filter);

const double* rexquad_GetFilterState(const rexquad_DelayedMEKF* filter);

const double* rexquad_GetStateEstimate(const rexquad_DelayedMEKF* filter);

const double* rexquad_GetPredictedState(const rexquad_DelayedMEKF* filter);

const double* rexquad_GetPredictedCovariance(const rexquad_DelayedMEKF* filter);

const double* rexquad_GetUpdatedState(const rexquad_DelayedMEKF* filter);

const double* rexquad_GetUpdatedCovariance(const rexquad_DelayedMEKF* filter);

const double* rexquad_GetDelayedState(const rexquad_DelayedMEKF* filter);

void rexquad_StatePrediction(rexquad_DelayedMEKF* filter, const double* xf,
                             const double* uf, const double* Pf, double h);

void rexquad_MeasurementUpdate(rexquad_DelayedMEKF* filter, const double* xf,
                               const double* Pf, const double* y_mocap);

void rexquad_UpdateStateEstimate(rexquad_DelayedMEKF* filter, const double* y_imu,
                                 const double* y_mocap, double h);