#ifndef __QCOPTER_CTRL_H
#define __QCOPTER_CTRL_H

#include "stm32f4xx.h"
#include "QuadCopterConfig.h"


#define MIN_PWM_INPUT 1000
#define MAX_PWM_INPUT 5000

#if USE_FUTABA_CONTROLLER
#define MAX_PWM1_INPUT 3766.0
#define MIN_PWM1_INPUT 2120.0
#define MID_PWM1_INPUT 3036.0

#define MAX_PWM2_INPUT 3854.0
#define MIN_PWM2_INPUT 2206.0
#define MID_PWM2_INPUT 3045.0

#define MAX_PWM3_INPUT 3853.0
#define MIN_PWM3_INPUT 2205.0
#define MID_PWM3_INPUT 2214.0

#define MAX_PWM4_INPUT 3872.0
#define MIN_PWM4_INPUT 2224.0
#define MID_PWM4_INPUT 3033.0

#define MAX_PWM5_INPUT 4145.0
#define MIN_PWM5_INPUT 1931.0
#endif

#if USE_WFLY_CONTROLLER
#define MAX_PWM1_INPUT 3900.0
#define MID_PWM1_INPUT 3040.0
#define MIN_PWM1_INPUT 2201.0

#define MAX_PWM2_INPUT 3900.0
#define MID_PWM2_INPUT 3040.0
#define MIN_PWM2_INPUT 2201.0

#define MAX_PWM3_INPUT 3900.0
#define MIN_PWM3_INPUT 2210.0

#define MAX_PWM4_INPUT 3900.0
#define MID_PWM4_INPUT 3040.0
#define MIN_PWM4_INPUT 2201.0

#define MAX_PWM5_INPUT 3900.0
#define MIN_PWM5_INPUT 2201.0
#endif

#if USE_DEVO_v10_CONTROLLER
#define MAX_PWM1_INPUT 3796.0
#define MID_PWM1_INPUT 2995.0 	// AILE // Roll
#define MIN_PWM1_INPUT 2196.0

#define MAX_PWM2_INPUT 3804.0
#define MID_PWM2_INPUT 3065.0 	// ELEV // Pitch
#define MIN_PWM2_INPUT 2203.0

#define MAX_PWM3_INPUT 3795.0
#define MIN_PWM3_INPUT 2195.0 	// THRO // Thrust

#define MAX_PWM4_INPUT 3804.0
#define MID_PWM4_INPUT 3004.0 	// RUDD // Yaw
#define MIN_PWM4_INPUT 2204.0

#define MAX_PWM5_INPUT 3800.0 	
#define MIN_PWM5_INPUT 2200.0 	// GEAR // Protect Switch
#endif

/*
	          left hand                          right hand
           _______________                     _______________
          /       ↑       \                   /       ↑       \
         /     PWM2(Pitch) \                 /    PWM3(Thrust) \    
        /         |         \               /         |         \
       /          |          \             /          |          \
       <<---------+--------->> PWM4(Yaw)   <<---------+--------->> PWM1(Roll) 
       \          |          /             \          |          /
        \         |         /               \         |         /
         \        |        /                 \        |        /
          \______ ↓ ______/                   \______ ↓ ______/
*/

#define MAX_CTRL_ROLL 45
#define MIN_CTRL_ROLL -45

#define MAX_CTRL_PITCH 45
#define MIN_CTRL_PITCH -45

#define MAX_CTRL_YAW 180
#define MIN_CTRL_YAW -180
typedef enum RC_State {
	GET_SIGNAL = 0x00,
	NO_SIGNAL = 0x01
} RC_State;
#define ENGINE_OFF 1
#define ENGINE_ON 0

void Update_RC_Control(int16_t *Roll, int16_t  *Pitch, int16_t  *Yaw, int16_t  *Thr, int16_t *safety); 
RC_State remote_signal_check();

extern vs16 PWM_M1;
extern vs16 PWM_M2;
extern vs16 PWM_M3;
extern vs16 PWM_M4;

#endif
