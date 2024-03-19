#include  "freertos_demo.h"
#include  "FreeRTOS.h"
#include  "task.h"
#include  "led.h"



void vTaskCode(void * pvParameters)
{

    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    for( ;; )
    {
        /* Task code goes here. */
    }
}



void freertos_demo(void)
{
    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;        //任务句柄
    xReturned =    xTaskCreate(
                    vTaskCode,       /* Function that implements the task. */
                    "NAME",          /* Text name for the task. */
                    12,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &xHandle );      /* Used to pass out the created task's handle. */

     if( xReturned == pdPASS )
    {
        /* The task was created.  Use the task's handle to delete the task. */
        vTaskDelete( xHandle );
    }
  



    // vTaskStartScheduler();      //开启任务调度器
}
