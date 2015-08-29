#include "Cpu0_Main.h"
#include "SysSe/Bsp/Bsp.h"
//#include "DemoApp.h"
#include "communication.h"



#include "Compilers.h"
#include "Cpu\Std\IfxCpu_Intrinsics.h"
#include "Port\Io\IfxPort_Io.h"
#include "Stm\Std\IfxStm.h"
#include "Src\Std\IfxSrc.h"
#include "Gtm\Std\IfxGtm.h"
#include "Gtm\Std\IfxGtm_Atom.h"
#include "Gtm\Std\IfxGtm_Tom.h"
#include "Gtm\Std\IfxGtm_Tim.h"
#include "Gtm\Tom\Timer\IfxGtm_Tom_Timer.h"
#include "Gtm\Tom\PwmHl\IfxGtm_Tom_PwmHl.h"
#include "Gtm\Trig\IfxGtm_Trig.h"
#include "IfxGtm_PinMap.h"
//#include "DemoApp.h


#define STM0_TICK_PERIOD_IN_MICROSECONDS 1000
#define IFX_CFG_ISR_PRIORITY_STM0_COMPARE0	10 /**< \brief Stm0 Compare 0 interrupt priority.  */
unsigned char Data_pointer = 0;
uint32 stm0CompareValue;
unsigned int Data_array[100];
uint32 Update_flag = 0;
uint32 Up_Down_flag = 0;
float32 PwmFrequency;
uint32 PwmPeriod;
uint32 PwmDuty;
IfxGtm_Cmu_Clk PwmGtmCmuClk = IfxGtm_Cmu_Clk_0;




App_Cpu0 g_AppCpu0; /**< \brief CPU 0 global data */
unsigned long  lock=1; // 1 means available,
unsigned long mask=1;


int core0_main (void)
{
	unsigned int i=0;
    unsigned int *p; 
    p=(unsigned int*)0xc0000000;
	*p++=0x000ccf85;
	*p++=0xcfa51fc2;
	*p++=0x9000000c;
	*p=0x000c5f85;
	
    /*
     * !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdog in the demo if it is required and also service the watchdog periodically
     * */
    IfxScuWdt_disableCpuWatchdog (IfxScuWdt_getCpuWatchdogPassword ());
    IfxScuWdt_disableSafetyWatchdog (IfxScuWdt_getSafetyWatchdogPassword ());

    /* Initialise the application state */
    g_AppCpu0.info.pllFreq = IfxScuCcu_getPllFrequency();
    g_AppCpu0.info.cpuFreq = IfxScuCcu_getCpuFrequency(IfxCpu_getCoreId());
    g_AppCpu0.info.sysFreq = IfxScuCcu_getSpbFrequency();
    g_AppCpu0.info.stmFreq = IfxStm_getFrequency(&MODULE_STM0);

    /* Enable the global interrupts of this CPU */
    IfxCpu_enableInterrupts();

    /* Demo init */
    // configure P33.8 as general output
    IfxPort_setPinMode(&MODULE_P33, 8,  IfxPort_Mode_outputPushPullGeneral);
    // configure P33.9 as general output
    IfxPort_setPinMode(&MODULE_P33, 9,  IfxPort_Mode_outputPushPullGeneral);

    IfxPort_setPinMode(&MODULE_P33, 10,  IfxPort_Mode_outputPushPullGeneral);
    // configure P33.9 as general output
    IfxPort_setPinMode(&MODULE_P33, 11,  IfxPort_Mode_outputPushPullGeneral);

    /* background endless loop */
    //while (1)
    {
    	//synchronizeCore0Core1();
    	//communicationCore0Core1_ptr->core0Ready = 1;
    	//IfxPort_togglePin(&MODULE_P33, 8);
    	//IfxPort_togglePin(&MODULE_P33, 9);
    	//IfxPort_togglePin(&MODULE_P33, 10);
    	//IfxPort_togglePin(&MODULE_P33, 11);
    	//IfxStm_waitTicks(&MODULE_STM0, g_AppCpu0.info.stmFreq/1000000);
    	//communicationCore0Core1_ptr->core0Ready = 0;
    	//IfxStm_waitTicks(&MODULE_STM0, g_AppCpu0.info.stmFreq/100);
    }

   // while(lock==0) requestLock(&lock, mask);
   // for(i=0;i<1000;i++)
    while(1)
    {
       //IfxPort_togglePin(&MODULE_P33, 8);
       //IfxPort_togglePin(&MODULE_P33, 9);

      // test_fun_share();
      // IfxStm_waitTicks(&MODULE_STM0, g_AppCpu0.info.stmFreq/100);
    }
    //releaseLock(&lock, mask);

    while(1);
    return (1);
}
//#pragma code_core_association  private1

void STM_Demo_init(void)
{
	/* Initialize STM for the triggers*/
	IfxStm_CompareConfig stmCompareConfig;

    // configure P33.8 as general output
    IfxPort_setPinMode(&MODULE_P33, 8,  IfxPort_Mode_outputPushPullGeneral);

    /* Calculate the compare value of STM0 */
	stm0CompareValue = IfxStm_getFrequency(&MODULE_STM0) / STM0_TICK_PERIOD_IN_MICROSECONDS;	/* 1ms */

	IfxStm_enableOcdsSuspend(&MODULE_STM0);

	/* Configure interrupt service requests for STM trigger outputs */
	//IfxSrc_init(&MODULE_SRC.STM[0].SR[0], IfxSrc_Tos_cpu0, IFX_CFG_ISR_PRIORITY_STM0_COMPARE0);
	//IfxSrc_enable(&MODULE_SRC.STM[0].SR[0]);
	stmCompareConfig.servProvider = IfxSrc_Tos_cpu0;

	/* Call the constructor of configuration */
	IfxStm_initCompareConfig(&stmCompareConfig);

	/* Modify only the number of ticks and enable the trigger output */
	stmCompareConfig.ticks = stm0CompareValue;   /*Interrupt after stm0CompareValue ticks from now */
	stmCompareConfig.triggerInterruptEnabled = IFX_CFG_ISR_PRIORITY_STM0_COMPARE0;

	/* Now Compare functionality is initialized */
	IfxStm_initCompare(&MODULE_STM0, &stmCompareConfig);

}
IFX_INTERRUPT(Ifx_STM0_Isr,0,IFX_CFG_ISR_PRIORITY_STM0_COMPARE0)
{
    uint32 stmTicks;
    stmTicks= (uint32)(stm0CompareValue * 1);
    IfxStm_updateCompare (&MODULE_STM0, IfxStm_Comparator_0, IfxStm_getCompare (&MODULE_STM0, IfxStm_Comparator_0) + stmTicks);
    IfxPort_togglePin(&MODULE_P33, 8);
    Data_pointer++;
    if(Data_pointer == 100) Data_pointer = 0;
    Data_array[Data_pointer] = MODULE_STM0.TIM0.U;
    __enable ();
    if(Update_flag==1){
    	if(Up_Down_flag == 1){
    		PwmFrequency-=10;
    	}else{
    		PwmFrequency+=10;
    	}

    	if(PwmFrequency != 0){
    	    PwmPeriod = (uint32)(IfxGtm_Cmu_getClkFrequency(&MODULE_GTM, PwmGtmCmuClk, FALSE) / PwmFrequency);
    	    PwmDuty = (PwmPeriod * 30) / 100;
    	}else{
    	    	PwmPeriod = 0x100;
    	    	PwmDuty = 0x0;
    	}

    	/*
    	if(Up_Down_flag == 1){
    		PwmPeriod-=100;
    	}else{
    		PwmPeriod+=100;
    	}
    	*/
    	//PwmDuty+=100;
    	if(PwmFrequency >= 20000) Up_Down_flag = 1;
    	if(PwmFrequency <= 50) Up_Down_flag = 0;

    	//PwmPeriod+=100;
    	//if(PwmFrequecncy>= 200000) PwmFrequency = 100000;
    	//IfxGtm_Atom_Ch_setCompareShadow(PwmGTMATOM, PwmGtmAtomCh, PwmPeriod, PwmDuty);
    }
    //CounterTick(IFX_OSTASK_COUNTER);
}


