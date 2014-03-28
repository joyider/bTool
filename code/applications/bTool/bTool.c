/**
 * @file bTool.c
 *   Test case 02 of RTuinOS. One task is defined, which runs alternatingly with the idle
 * task.\n
 *   Observations:\n
 *   The object Serial is used to write progress and status information to console, under
 * which the current CPU load. It is remarkably high, which is not the consumption of the
 * software implemented here but the effect of the data streaming over the RS 232
 * connection. We have selected a Baud rate of only 9600 bps and all print command block
 * until the characters to print are processed. At the beginning it is very fast as the
 * characters immediately fit into the send buffer. Once is has been filled the print
 * function is in mean as fast as the stream, 9600 characters a second. Even though the
 * rest is just waiting somewhere inside print it's lost CPU processing time for RTuinOS. A
 * hypothetical redesign of the library for serial communication for RTuinOS would
 * obviously use a suspend command to free this time for use by other tasks. This way, the
 * mean CPU load would become independent of the chosen Baud rate.\n
 *   Please consider to change the Baud rate to 115200 bps in setup() to prove that the CPU
 * load strongly goes down.
 *
 * Copyright (C) 2012-2013 Peter Vranken (mailto:Peter_Vranken@Yahoo.de)
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Module interface
 *   setup
 *   loop
 * Local functions
 *   blink
 *   task01_class00
 */

/*
 * Include files
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "rtos.h"
#include "rtos_assert.h"
#include "gsl_systemLoad.h"
#include "dht11.h"  //Import needed libraries


dht11 DHT11;  //Declare objects


/*
 * Defines
 */
 
/** Pin Setup Arduino borad PIN for tempsensor_1. */
#define TEMP_SENSOR_1 2
 
/** Stack size of task. */
#define STACK_SIZE_TASK_TEMP_SENSORS   256
#define STACK_SIZE_TASK_LCD_MANAGEMENT   256
 

/*
 * Local type definitions
 */
 
 
/*
 * Local prototypes
 */
 
static void task_Temp_sensors(uint16_t taskParam);
static void task_LCD_management(uint16_t taskParam);
 
int _last_temp=0;
 
/*
 * Data definitions
 */
 
static uint8_t _taskStack_Temp_sensors[STACK_SIZE_TASK_TEMP_SENSORS];
static uint8_t _taskStack_LCD_management[STACK_SIZE_TASK_LCD_MANAGEMENT];
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
    
    Serial.print("task01_class00: Activated by 0x");
    Serial.println(initCondition, HEX);


    
    for(;;)
    {
    	//u = rtos_delay(500);
    	get_Data_From_sensor();
    	 Serial.print("Humidity (%): ");
    	  Serial.println((float)DHT11.humidity, 2);

    	  Serial.print("Temperature (oC): ");
    	  Serial.println((float)DHT11.temperature, 2);

    	  Serial.print("Dew Point (oC): ");
    	  Serial.println(dewPoint(DHT11.temperature, DHT11.humidity));

    	  Serial.print("Dew PointFast (oC): ");
    	  Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));
    	  //u = rtos_suspendTaskTillTime(/* deltaTimeTillRelease */ 5000);
    }
    
#undef TICS_CYCLE
} /* End of task_Temp_sensors */

static void task_LCD_management(uint16_t initCondition)

{
	 /* initialize the library with the numbers of the interface pins */
	    LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

	for (;;)
	{
			lcd.begin(20, 4);                      // set up the LCD's number of columns and rows
			  lcd.clear();                           // Clear LCD
			  lcd.setCursor(0,0);                    // Set cursor to x=0 and y=0
			  lcd.print("Temperature: ");           // Print text on LCD
			  lcd.setCursor(13,0);
			  lcd.print("boOS");
			  lcd.setCursor(0,2);
			  lcd.print("bTool v:0.1a");
			  lcd.setCursor(0,3);
			  lcd.print("Fermentor Status");
			  lcd.display();
			  //rtos_suspendTaskTillTime(/* deltaTimeTillRelease */ 5000);
			  rtos_delay(500);
	}

}


/**
 * The initalization of the RTOS tasks and general board initialization.
 */ 

void setup(void)
{
    /* Start serial port at 9600 bps. */
    Serial.begin(9600);
    Serial.println("\n" RTOS_RTUINOS_STARTUP_MSG);



    
    /* Configure task 1 of priority class 0 */
        uint8_t idxTask = 0;

    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     task_Temp_sensors
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStack_Temp_sensors[0]
                       , /* stackSize */        sizeof(_taskStack_Temp_sensors)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     5
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                          , /* taskFunction */     task_LCD_management
                          , /* prioClass */        0
                          , /* pStackArea */       &_taskStack_LCD_management[0]
                          , /* stackSize */        sizeof(_taskStack_LCD_management)
                          , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                          , /* startByAllEvents */ false
                          , /* startTimeout */     5
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

	//get_Data_From_sensor();
    /* Share current CPU load measurement with task code, which owns Serial and which can
       thus display it. */
    _cpuLoad = gsl_getSystemLoad();
    
} /* End of loop */




