#include "delay.h"
#include "sys.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS

#include  "FreeRTOS.h"	 					//ucos 使用	  
#include  "task.h"
//#include "FreeRTOSConfig.h"

#endif


static u32 fac_us=0;							//us延时倍乘数
static u16 fac_ms=0;							//ms延时倍乘数,在os下,代表每个节拍的ms数

extern void xPortSysTickHandler(void); 
 
 
//systick中断服务函数,使用OS时用到
void SysTick_Handler(void)
{	 
    HAL_IncTick(); 
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
    {
        //OS开跑了，才能执行正常的调度处理
        xPortSysTickHandler();  
    }
 
}
	




//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟固定为AHB时钟的1/8
//SYSCLK:系统时钟频率
void delay_init(u16 SYSCLK)
{

#if SYS_SUPPORT_OS
    u32 reload;
#endif
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    fac_us = SYSCLK;
 #if SYS_SUPPORT_OS   
    reload=SYSCLK;		  
    reload *= 1000000/  configTICK_RATE_HZ;
    
    SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;//开启SYSTICK中断
	SysTick->LOAD=reload; 					//每1/OS_TICKS_PER_SEC秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; //开启SYSTICK
#endif
}								    

#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
//延时nus
//nus:要延时的us数.	
//nus:0~204522252(最大值即2^32/fac_us@fac_us=21)	    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us;  						//需要的节拍数 
                                //	delay_osschedlock();					//阻止OS调度，防止打断us延时
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
                                //  delay_osschedunlock();					//恢复OS调度											    
}  
//延时nms
//nms:要延时的ms数
//nms:0~65535
void delay_ms(u16 nms)
{	 
    uint32_t i;
    for(i=0;i<nms;i++)
    {
        delay_us(1000);
    }
}






#else  //不用ucos时
//延时nus
//nus为要延时的us数.	
//注意:nus的值不要大于1000us
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
}
 
//延时nms
//nms:要延时的ms数
void delay_ms(u16 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);
}

#endif
			 



































