/* 
* @Author: joyider
* @Date:   2014-03-31 11:54:52
* @Last Modified by:   joyider
* @Last Modified time: 2014-04-06 20:55:25
*/

#define SEM_0                           (RTOS_EVT_SEMAPHORE_00)
#define SEM_1                           (RTOS_EVT_SEMAPHORE_01)
                                        
#define MTX_0                           (RTOS_EVT_MUTEX_02)
#define MTX_1                           (RTOS_EVT_MUTEX_03)
                                        
#define EVT_0                           (RTOS_EVT_EVENT_04)
#define EVT_1                           (RTOS_EVT_EVENT_05)

/** This event is sent by the control task to trigger the next step. The three events need
    to be of neighboured indexes, as they are used by index, like
    (\a EVT_TRIGGER_TASK << \a idxTask). */
#define EVT_TRIGGER_bTool                			(RTOS_EVT_EVENT_06)
#define EVT_TRIGGER_Temp_sensors                  	(RTOS_EVT_EVENT_06)
#define EVT_TRIGGER_LCD_management                  (RTOS_EVT_EVENT_07)
#define EVT_TRIGGER_T0                  			(RTOS_EVT_EVENT_08)

    