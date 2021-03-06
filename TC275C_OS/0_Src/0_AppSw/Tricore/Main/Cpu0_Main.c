#include "Cpu0_Main.h"
#include "SysSe/Bsp/Bsp.h"
//#include "DemoApp.h"
#include "communication.h"



#include "Compilers.h"
#include "Cpu\Std\IfxCpu_Intrinsics.h"
#include "Port\Io\IfxPort_Io.h"
#include "Stm\Std\IfxStm.h"
#include "Src\Std\IfxSrc.h"

#include "os_kernel.h"

#define STM0_TICK_PERIOD_IN_MICROSECONDS    1000
#define STM1_TICK_PERIOD_IN_MICROSECONDS    1000
#define STM2_TICK_PERIOD_IN_MICROSECONDS    1000

#define IFX_CFG_ISR_PRIORITY_CPU0_SOFTWAR0	9    /**< \brief Stm0 Compare 0 interrupt priority.  */
#define IFX_CFG_ISR_PRIORITY_CPU0_SOFTWAR1	20   /**< \brief Stm0 Compare 0 interrupt priority.  */

#define IFX_CFG_ISR_PRIORITY_STM0_COMPARE0	10   /**< \brief Stm0 Compare 0 interrupt priority.  */
#define IFX_CFG_ISR_PRIORITY_STM1_COMPARE0	11   /**< \brief Stm1 Compare 0 interrupt priority.  */
#define IFX_CFG_ISR_PRIORITY_STM2_COMPARE0	12   /**< \brief Stm2 Compare 0 interrupt priority.  */

#define IFX_CFG_ISR_PRIORITY_STM0_COMPARE1	20   /**< \brief Stm0 Compare 0 interrupt priority.  */
#define IFX_CFG_ISR_PRIORITY_STM1_COMPARE1	21   /**< \brief Stm1 Compare 0 interrupt priority.  */
#define IFX_CFG_ISR_PRIORITY_STM2_COMPARE1	22   /**< \brief Stm2 Compare 0 interrupt priority.  */

uint32 stm0CompareValue;
uint32 stm1CompareValue;
uint32 stm2CompareValue;

uint32 stm0CompareValue2;


App_Cpu0 g_AppCpu0; /**< \brief CPU 0 global data */
unsigned long  lock=1; // 1 means available,
unsigned long mask=1;

#if 0
inline void update_stm0_ticks(void)
{
    uint32 stmTicks;

    stmTicks= (uint32)(stm0CompareValue*10);
    IfxStm_updateCompare (&MODULE_STM0, IfxStm_Comparator_0, IfxStm_getCompare (&MODULE_STM0, IfxStm_Comparator_0) + stmTicks);
    IfxPort_togglePin(&MODULE_P33, 10);
}
#endif

/**********************************************************************************
 *
 *
 *   STM0 initialization and interrupt
 *
 *
 *
 *********************************************************************************/


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

void STM_Demo_init_stm0_compare1(void)
{
	/* Initialize STM for the triggers*/
	IfxStm_CompareConfig stmCompareConfig;

    // configure P33.8 as general output
    //IfxPort_setPinMode(&MODULE_P33, 8,  IfxPort_Mode_outputPushPullGeneral);

    /* Calculate the compare value of STM0 */
	stm0CompareValue2 = IfxStm_getFrequency(&MODULE_STM0) / STM0_TICK_PERIOD_IN_MICROSECONDS;	/* 1ms */

	IfxStm_enableOcdsSuspend(&MODULE_STM0);

	/* Configure interrupt service requests for STM trigger outputs */
	//IfxSrc_init(&MODULE_SRC.STM[0].SR[0], IfxSrc_Tos_cpu0, IFX_CFG_ISR_PRIORITY_STM0_COMPARE0);
	//IfxSrc_enable(&MODULE_SRC.STM[0].SR[0]);
	stmCompareConfig.servProvider = IfxSrc_Tos_cpu0;

	/* Call the constructor of configuration */
	IfxStm_initCompareConfig(&stmCompareConfig);

	/* Modify only the number of ticks and enable the trigger output */
	stmCompareConfig.ticks = stm0CompareValue2;   /*Interrupt after stm0CompareValue ticks from now */
	stmCompareConfig.triggerInterruptEnabled = IFX_CFG_ISR_PRIORITY_STM0_COMPARE1;

	/* Use the compare1 */
    stmCompareConfig.comparator            = IfxStm_Comparator_1;
	stmCompareConfig.comparatorInterrupt   = IfxStm_ComparatorInterrupt_ir1;
	
	/**< \brief Comparator start bit position  defined in MODULE_STMx.CMCON.B.MSTART0(x = 0,1,2). */
	stmCompareConfig.compareOffset         = IfxStm_ComparatorOffset_12;  
	/**< \brief Size of compare value to compare with timer defined in  MODULE_STMx.CMCON.B.MSIZE0(x = 0,1,2). */
    stmCompareConfig.compareSize           = IfxStm_ComparatorSize_16Bits;             
	/* Now Compare functionality is initialized */
	
	IfxStm_initCompare(&MODULE_STM0, &stmCompareConfig);
	IfxStm_disableIsrRequest(&MODULE_STM0);


}

/**********************************************************************************
 *
 *
 *   STM1 initialization and interrupt
 *
 *
 *
 *********************************************************************************/
void STM1_Demo_init(void)
{
	/* Initialize STM for the triggers*/
	IfxStm_CompareConfig stmCompareConfig;

    // configure P33.8 as general output
    IfxPort_setPinMode(&MODULE_P33, 9,  IfxPort_Mode_outputPushPullGeneral);

    /* Calculate the compare value of STM0 */
	stm1CompareValue = IfxStm_getFrequency(&MODULE_STM1) / STM1_TICK_PERIOD_IN_MICROSECONDS;	/* 1ms */

	IfxStm_enableOcdsSuspend(&MODULE_STM1);

	/* Configure interrupt service requests for STM trigger outputs */
	//IfxSrc_init(&MODULE_SRC.STM[0].SR[0], IfxSrc_Tos_cpu0, IFX_CFG_ISR_PRIORITY_STM0_COMPARE0);
	//IfxSrc_enable(&MODULE_SRC.STM[0].SR[0]);
	stmCompareConfig.servProvider = IfxSrc_Tos_cpu1;

	/* Call the constructor of configuration */
	IfxStm_initCompareConfig(&stmCompareConfig);

	/* Modify only the number of ticks and enable the trigger output */
	stmCompareConfig.ticks = stm1CompareValue;   /*Interrupt after stm0CompareValue ticks from now */
	stmCompareConfig.triggerInterruptEnabled = IFX_CFG_ISR_PRIORITY_STM1_COMPARE0;

	/* Now Compare functionality is initialized */
	IfxStm_initCompare(&MODULE_STM1, &stmCompareConfig);

}

IFX_INTERRUPT(Ifx_STM1_Isr,0,IFX_CFG_ISR_PRIORITY_STM1_COMPARE0)
{
    uint32 stmTicks;
    stmTicks= (uint32)(stm1CompareValue * 100);
    IfxStm_updateCompare (&MODULE_STM1, IfxStm_Comparator_0, IfxStm_getCompare (&MODULE_STM1, IfxStm_Comparator_0) + stmTicks);
    //IfxPort_togglePin(&MODULE_P33, 9);
}

/**********************************************************************************
 *
 *
 *   STM2 initialization and interrupt
 *
 *
 *
 *********************************************************************************/
void STM2_Demo_init(void)
{
	/* Initialize STM for the triggers*/
	IfxStm_CompareConfig stmCompareConfig;

    // configure P33.8 as general output
    IfxPort_setPinMode(&MODULE_P33, 10,  IfxPort_Mode_outputPushPullGeneral);

    /* Calculate the compare value of STM0 */
	stm2CompareValue = IfxStm_getFrequency(&MODULE_STM2) / STM2_TICK_PERIOD_IN_MICROSECONDS;	/* 1ms */

	IfxStm_enableOcdsSuspend(&MODULE_STM2);

	/* Configure interrupt service requests for STM trigger outputs */
	//IfxSrc_init(&MODULE_SRC.STM[0].SR[0], IfxSrc_Tos_cpu0, IFX_CFG_ISR_PRIORITY_STM0_COMPARE0);
	//IfxSrc_enable(&MODULE_SRC.STM[0].SR[0]);
	stmCompareConfig.servProvider = IfxSrc_Tos_cpu2;

	/* Call the constructor of configuration */
	IfxStm_initCompareConfig(&stmCompareConfig);

	/* Modify only the number of ticks and enable the trigger output */
	stmCompareConfig.ticks = stm2CompareValue;   /*Interrupt after stm0CompareValue ticks from now */
	stmCompareConfig.triggerInterruptEnabled = IFX_CFG_ISR_PRIORITY_STM2_COMPARE0;

	/* Now Compare functionality is initialized */
	IfxStm_initCompare(&MODULE_STM2, &stmCompareConfig);

}

IFX_INTERRUPT(Ifx_STM2_Isr,0,IFX_CFG_ISR_PRIORITY_STM2_COMPARE0)
{
    uint32 stmTicks;
    stmTicks= (uint32)(stm0CompareValue * 100);
    IfxStm_updateCompare (&MODULE_STM2, IfxStm_Comparator_0, IfxStm_getCompare (&MODULE_STM2, IfxStm_Comparator_0) + stmTicks);
    //IfxPort_togglePin(&MODULE_P33, 10);
}

int cpu0_software_interrupt_test_in_interrupt;
int cpu0_software_interrupt_test_in_interrupt1;
int cpu0_software_interrupt_test_in_main_loop;

void Init_soft_interrupt(volatile Ifx_SRC_SRCR *src,int priority)
{
	src->B.SRE=1;
	src->B.TOS=0;
	src->B.SRPN=priority;
}

void trigger_soft_interrupt(volatile Ifx_SRC_SRCR *src)
{
    src->B.SETR=1;
    cpu0_software_interrupt_test_in_main_loop++;
}
 //   	trigger_soft_interrupt(SRC_GPSR00)


//IFX_INTERRUPT(CPU0_SOFT0_Isr,0,IFX_CFG_ISR_PRIORITY_CPU0_SOFTWAR0)
//{
    //uint32 stmTicks;
    //stmTicks= (uint32)(stm0CompareValue * 100);
    //IfxStm_updateCompare (&MODULE_STM2, IfxStm_Comparator_0, IfxStm_getCompare (&MODULE_STM2, IfxStm_Comparator_0) + stmTicks);
    //IfxPort_togglePin(&MODULE_P33, 10);
	//cpu0_software_interrupt_test_in_interrupt++;
//}

//IFX_INTERRUPT(CPU0_SOFT1_Isr,0,IFX_CFG_ISR_PRIORITY_CPU0_SOFTWAR1)
//{
    //uint32 stmTicks;
    //stmTicks= (uint32)(stm0CompareValue * 100);
    //IfxStm_updateCompare (&MODULE_STM2, IfxStm_Comparator_0, IfxStm_getCompare (&MODULE_STM2, IfxStm_Comparator_0) + stmTicks);
    //IfxPort_togglePin(&MODULE_P33, 10);
//	cpu0_software_interrupt_test_in_interrupt1++;
//}

/**********************************************************************************
 *
 *
 *   Core0 main function
 *
 *
 *
 *********************************************************************************/
uint32 core0_switch_context_count_test;

//#if 0
int math_return;
int math_return2;

int math3(int a,int b)
{
   int w;

    w=a+b;
    return w;
}

int math2(int a,int b,int c)
{
   int w;

   w=a+b+c;
   w=w+math3(9,10);
   return w;
}

int math(int a,int b,int c,int d,int e,int f,int g,int h,int i)
{
   int w;
   int z;
   int q;
   int l;

    w=a+b+c+d+e*2+f*3+g+h+10*i;

    z=math2(6,7,w);

    q=w+math3(7,w);

    l=z+q;

    return l;
}
//#endif

int core0_main (void)
{
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

    STM_Demo_init();
   // STM_Demo_init_stm0_compare1();
    //STM1_Demo_init();
    //STM2_Demo_init();

    /* Enable the global interrupts of this CPU */
    IfxCpu_enableInterrupts();

    /* Demo init */
    // configure P33.8 as general output
    IfxPort_setPinMode(&MODULE_P33, 8,  IfxPort_Mode_outputPushPullGeneral);
     //configure P33.9 as general output
    IfxPort_setPinMode(&MODULE_P33, 9,  IfxPort_Mode_outputPushPullGeneral);

    IfxPort_setPinMode(&MODULE_P33, 10,  IfxPort_Mode_outputPushPullGeneral);
    // configure P33.9 as general output
    IfxPort_setPinMode(&MODULE_P33, 11,  IfxPort_Mode_outputPushPullGeneral);

    //Init_soft_interrupt(&SRC_GPSR00,IFX_CFG_ISR_PRIORITY_CPU0_SOFTWAR0);
    //Init_soft_interrupt(&SRC_GPSR01,IFX_CFG_ISR_PRIORITY_CPU0_SOFTWAR1);
//#if 0
    int a=1;
    int b=2;
    int c=3;
    int d=4;
    int e=5;


    math_return=math(1,2,3,4,5,6,7,8,9);
    math_return2=math(1,2,3,4,5,6,7,810,3);
//#endif

    /* background endless loop */
   // start_core0_os();

    while (1)
    {
    	//synchronizeCore0Core1();
    	//communicationCore0Core1_ptr->core0Ready = 1;
    	//IfxPort_togglePin(&MODULE_P33, 8);
    	//IfxPort_togglePin(&MODULE_P33, 9);
    	//IfxPort_togglePin(&MODULE_P33, 10);
    	//IfxPort_togglePin(&MODULE_P33, 11);
    	//switch_context();
    	//core0_switch_context_count_test++;
    	//trigger_soft_interrupt(&SRC_GPSR00);
    	//trigger_soft_interrupt(&SRC_GPSR00);
    	//trigger_soft_interrupt(&SRC_GPSR01);
        IfxStm_waitTicks(&MODULE_STM0, 10000000);

    	//IfxStm_waitTicks(&MODULE_STM0, g_AppCpu0.info.stmFreq/1000000);
    	//communicationCore0Core1_ptr->core0Ready = 0;
    	//IfxStm_waitTicks(&MODULE_STM0, g_AppCpu0.info.stmFreq/100);
    }

   // while(lock==0) requestLock(&lock, mask);
   // for(i=0;i<1000;i++)
#if 0
    while(1)
    {
       //IfxPort_togglePin(&MODULE_P33, 8);
       //IfxPort_togglePin(&MODULE_P33, 9);

      // IfxStm_waitTicks(&MODULE_STM0, g_AppCpu0.info.stmFreq/100);
    }
    //releaseLock(&lock, mask);
#endif

    return (1);
}



