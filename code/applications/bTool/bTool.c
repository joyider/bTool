/* 
* @Author: joyider
* @Date:   2014-03-29 23:21:15
* @Last Modified by:   joyider
* @Last Modified time: 2014-04-06 20:55:19
*/


#include <Arduino.h>
#include <LiquidCrystal.h>
#include "rtos.h"
#include "rtos_assert.h"
#include "gsl_systemLoad.h"
#include "dht11.h"  //Import needed libraries
#include "bTool.h"


dht11 DHT11;  //Declare objects


/*
 * Defines
 */
 
/** Pin Setup Arduino borad PIN for tempsensor_1. */
#define TEMP_SENSOR_1 2
 
/** Stack size of task. */
#define STACK_SIZE_TASK_TEMP_SENSORS   256
#define STACK_SIZE_TASK_LCD_MANAGEMENT   256
#define STACK_SIZE_TASK_RELAY_CTRL   256
#define STACK_SIZE_TASK_CONTROL   256
#define STACK_SIZE_TASK_SERIAL   256
#define STACK_SIZE_TASK_HERMS   256
#define STACK_SIZE_TASK_FERMENTATION   256


/*
 * Local type definitions
 */
 
 
/*
 * Local prototypes
 */
 
static void task_Temp_sensors(uint16_t taskParam);
static void task_LCD_management(uint16_t taskParam);
static void task_Control(uint16_t taskParam);
static void task_Relay_CTRL(uint16_t taskParam);
static void task_Serial(uint16_t taskParam);
static void task_HERMS(uint16_t taskParam);
static void task_FERMENTATION(uint16_t taskParam);
 
int _last_temp=0,
    _step;
 
/*
 * Data definitions
 */
 
static uint8_t _taskStack_Temp_sensors[STACK_SIZE_TASK_TEMP_SENSORS];
static uint8_t _taskStack_LCD_management[STACK_SIZE_TASK_LCD_MANAGEMENT];
static uint8_t _taskStack_Control[STACK_SIZE_TASK_CONTROL];
static uint8_t _taskStack_Relay_Ctrl[STACK_SIZE_TASK_RELAY_CTRL];
static uint8_t _taskStack_Serial[STACK_SIZE_TASK_SERIAL];
static uint8_t _taskStack_HERMS[STACK_SIZE_TASK_HERMS];
static uint8_t _taskStack_FERMENTATION[STACK_SIZE_TASK_FERMENTATION];
static uint8_t _cpuLoad = 200;


/*
 * Function implementation
 */

/**
 * Trivial routine that flashes the LED a number of times to give simple feedback. The
 * routine is blocking.
 *   @param noFlashes
 * The number of times the LED is lit.
 */
 
static  void get_Data_From_sensor(void)
{
	Serial.println("\n");

	  int chk = DHT11.read(TEMP_SENSOR_1);

	  Serial.print("Read sensor: ");
	  switch (chk)
	  {
	    case 0: Serial.println("OK"); break;
	    case -1: Serial.println("Checksum error"); break;
	    case -2: Serial.println("Time out error"); break;
	    default: Serial.println("Unknown error"); break;
	  }


}
double dewPoint(double celsius, double humidity)
{
        double A0= 373.15/(273.15 + celsius);
        double SUM = -7.90298 * (A0-1);
        SUM += 5.02808 * log10(A0);
        SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
        SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
        SUM += log10(1013.246);
        double VP = pow(10, SUM-3) * humidity;
        double T = log(VP/0.61078);   // temp var
        return (241.88 * T) / (17.558-T);
}

// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
        double a = 17.271;
        double b = 237.7;
        double temp = (a * celsius) / (b + celsius) + log(humidity/100);
        double Td = (b * temp) / (a - temp);
        return Td;
}

static void task_Relay_CTRL(uint16_t taskParam)
{

}
static void task_Serial(uint16_t taskParam)
{

}
static void task_HERMS(uint16_t taskParam)
{

}
static void task_FERMENTATION(uint16_t taskParam)
{

}

/**
 * The only task in this test case (besides idle).
 *   @param initCondition
 * The task gets the vector of events, which made it initially due.
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void task_Temp_sensors(uint16_t initCondition)

{
#define TICS_CYCLE  125

    uint16_t u;
    uint32_t ti = millis()
           , tiCycle;
    int      current_temp=0;
    
    Serial.print("task01_class00: Activated by 0x");
    Serial.println(initCondition, HEX);


    
    for(;;)
    {
      Serial.println("Pre");
      rtos_waitForEvent(EVT_TRIGGER_bTool, /* all */ false, /* timeout */ 0);
      uint16_t reqEvtVec, gotEvtVec; //Outside loop?
    	rtos_delay(500);
      //rtos_suspendTaskTillTime(10000);
      Serial.println("pre temp");
    	get_Data_From_sensor();
      _last_temp=(int)DHT11.temperature;
      Serial.println("temp");
      if (current_temp != _last_temp)
      {
        current_temp=_last_temp;
        reqEvtVec = EVT_0;
        rtos_sendEvent(reqEvtVec); 
      }
    }
    
#undef TICS_CYCLE
} /* End of task_Temp_sensors */

static void task_LCD_management(uint16_t initCondition)

{
	 /* initialize the library with the numbers of the interface pins */
	    LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
      lcd.begin(20, 4);                      // set up the LCD's number of columns and rows
       lcd.clear();    
      lcd.setCursor(0,0);                    // Set cursor to x=0 and y=0
       lcd.print("Temperature: "); 
	do
	{
    uint16_t reqEvtVec, gotEvtVec; //move outeside loop?
    //rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_LCD_management, /* all */ false, /* timeout */ 0);
    //reqEvtVec = EVT_0;
    //gotEvtVec = rtos_waitForEvent(reqEvtVec, /* all */ true, /* timeout */ 0);
    //ASSERT(reqEvtVec == gotEvtVec);
    Serial.println("LCD");
		lcd.setCursor(13,0);
    lcd.print((int)_last_temp);
	}
  while(rtos_waitForEvent(EVT_0,/* all */ false, /* timeout */ 0));

}

static void task_Control(uint16_t initCondition)
{
  Serial.println("1:st control");
do
    {

        /* Trigger one task. */
        uint16_t evtTriggerTask = EVT_TRIGGER_Temp_sensors;
        rtos_sendEvent(evtTriggerTask);
        
        /* The test is cyclically repeated. */
        
        Serial.println("control");
        /* Any task may query the task overrun counter and this task is known to be
           regular. So we double-check the counter. */
        ASSERT(rtos_getTaskOverrunCounter(2, /* doReset */ false) == 0);
    }
    while(rtos_suspendTaskTillTime(/* deltaTimeTillResume */ 100));
}

/**
 * The initalization of the RTOS tasks and general board initialization.
 */ 

void setup(void)
{
    /* Start serial port at 9600 bps. */
    Serial.begin(9600);
    Serial.println("\n" RTOS_RTUINOS_STARTUP_MSG);



    
    
        uint8_t idxTask = 0;

    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     task_Temp_sensors
                       , /* prioClass */        1
                       , /* pStackArea */       &_taskStack_Temp_sensors[0]
                       , /* stackSize */        sizeof(_taskStack_Temp_sensors)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     5
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     task_LCD_management
                       , /* prioClass */        2
                       , /* pStackArea */       &_taskStack_LCD_management[0]
                       , /* stackSize */        sizeof(_taskStack_LCD_management)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     10
                       );
    /* Configure the control task of priority class 0. */
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     task_Control
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStack_Control[0]
                       , /* stackSize */        sizeof(_taskStack_Control)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     task__Relay_Ctrl
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStack_Relay_Ctrl[0]
                       , /* stackSize */        sizeof(_taskStack_Relay_Ctrl)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     task_Serial
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStack_Serial[0]
                       , /* stackSize */        sizeof(_taskStack_Serial)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     task_HERMS
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStack_HERMS[0]
                       , /* stackSize */        sizeof(_taskStack_HERMS)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     task_FERMENTATION
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStack_FERMENTATION[0]
                       , /* stackSize */        sizeof(_taskStack_FERMENTATION)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
} /* End of setup */




/**
 * The application owned part of the idle task. This routine is repeatedly called whenever
 * there's some execution time left. It's interrupted by any other task when it becomes
 * due.
 *   @remark
 * Different to all other tasks, the idle task routine may and should return. (The task as
 * such doesn't terminate). This has been designed in accordance with the meaning of the
 * original Arduino loop function.
 */ 

void loop(void)
{

    /* Share current CPU load measurement with task code, which owns Serial and which can
       thus display it. */
    _cpuLoad = gsl_getSystemLoad();
    Serial.println(_cpuLoad);
    
} /* End of loop */




