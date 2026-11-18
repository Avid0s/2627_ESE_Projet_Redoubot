/*
 * FSM.c
 *
 *  Created on: 18 nov. 2026
 *      Author: joham
 */

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"

/*-------------------------------------------------------
Event flags
-------------------------------------------------------*/

#define FLAG_TARGET_REACHED   (1U << 0)
#define FLAG_YAW_REACHED      (1U << 1)
#define FLAG_BATT_CRITICAL    (1U << 2)

/*-------------------------------------------------------
Énums d'états, commandes, etc
-------------------------------------------------------*/

typedef enum {
    STATE_INIT = 0,        
    STATE_SEARCH,
    STATE_APPROACH,
    STATE_GRAB_AND_READ,
    STATE_GOTO_ZONE,
    STATE_DROP,
    STATE_CRITICAL_ERROR
} RobotState_t;

typedef enum {
    MOTION_STOP = 0,
    MOTION_ROTATE_SCAN,
    MOTION_GOTO_NEXT_WP,
    MOTION_ALIGN_TOF_MIN,
    MOTION_FORWARD_SLOW,
    MOTION_ROTATE_TO_YAW,
    MOTION_GOTO_XY,
    MOTION_REVERSE_15CM
} MotionMode_t;

typedef enum {
    COLOR_UNKNOWN = 0,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW
} CanColor_t;

typedef enum {
    SND_START = 1,
    SND_FAIL  = 2,
    SND_VALID = 3,
    SND_DROP  = 4,
    SND_ALARM = 5
} AudioTrack_t;

typedef enum {
    MSG_TYPE_LOG = 0,      // USART2
    MSG_TYPE_AUDIO_TRACK   // USART3
} TelemetryType_t;

/*--------------------------------------------
// Structures de données
--------------------------------------------*/

// Consigne mouvement & actionneur
typedef struct {
    MotionMode_t mode;          // Mode d'asservissement demandé par la FSM
    float        target_x_m;    // Cible X (pour GOTO_XY / GOTO_NEXT_WP)
    float        target_y_m;    // Cible Y
    float        target_yaw_rad;// Cap cible (pour ROTATE_TO_YAW)
    bool         gripper_closed;// true = CLOSE, false = OPEN (SERVOL/SERVOR)
} MotionCmd_t;

// Odométrie
typedef struct {
    float x_m;
    float y_m;
    float yaw_rad;
} RobotPose_t;

// Données capteurs
typedef struct {
    uint16_t   tof_dist_mm;
    CanColor_t detected_color;
    uint8_t    battery_soc_pct; // Pourcentage batterie (BQ27220)
    bool       sensors_ok;
} SensorData_t;

// Messages télémétrie
typedef struct {
    TelemetryType_t type;
    AudioTrack_t    audio_track;
    char            text[64];
} TelemetryMsg_t;

/*-----------------------------------------
External handles
-----------------------------------------*/

extern SemaphoreHandle_t  mtxMotionCmd;
extern QueueHandle_t      qTelemetry;
extern EventGroupHandle_t egRobotEvents;
extern MotionCmd_t        g_MotionCmd;

void Robot_LogBLE(const char *format, ...);
void Robot_PlaySound(AudioTrack_t track);

typedef struct {
    RobotState_t current_state;
    RobotState_t previous_state; 
    uint32_t     state_enter_ms; 
    CanColor_t   detected_color; 
    uint16_t        target_x;
    uint16_t        target_y;
} FSM_t;

int FSM_init(){

}

redoubot_state_t redoubot_transition(FSM_t *fsm, redoubot_event_t evt){
    bool is_entry = (fsm->current_state != fsm->previous_state);    
    fsm->previous_state = fsm->current_state;
    switch(fsm->current_state){
        case STATE_INIT: 
            if(is_init_OK()){                               //on initialise tt avant, on vérifie juste que c'est bien init
                QueueLog("Init Success; Starting Research");
                fsm->previous_state = fsm->current_state;
                fsm->current_state  = STATE_SEARCH; 
            }
            else{
                QueueLog("Init Failed; Going to Critical Error");
                fsm->previous_state = fsm->current_state;
                fsm->current_state  = STATE_CRITICAL_ERROR; 
            }
            break;
        case STATE_SEARCH:
            if (is_entry){
                SetMotion(ROTATE_SCAN);
            }
            if (Can_Found()){
                QueueLog("Can Found; Moving towards it");
                fsm->current_state  = STATE_APPROACH; 
            }
            if (tour_complet()){
                QueueLog("Can not found; Go to critical Error");
                fsm->current_state  = STATE_CRITICAL_ERROR;
            }
            break;
        case STATE_APPROACH:
            if (is_entry){
                SetMotion(MOTION_GOTO_XY,fsm->target_x, fsm->target_y);
            }
            break;
        case STATE_GRAB_AND_READ:
            break;  
        case STATE_GOTO_ZONE:
            break;
        case STATE_DROP:
            break;
        case STATE_CRITICAL_ERROR:
            break;
    }
}

typedef enum {}redoubot_guard_t;
typedef enum {}redoubot_action_t;