#include "sys.h"

void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	// FreeRTOS on Cortex-M3 requires all implemented priority bits as preemption priority.

}
