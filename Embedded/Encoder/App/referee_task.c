#include "referee_task.h"
#include "bsp_log.h"
#include "cmsis_os.h"
#include "dev_referee.h"

RefereeInitConfig_s ref_config = {
.topic_name = "referee",
.uart_handle = &huart6,
};

RefereeInstance_s* ref_instance;
void referee_task(void const * argument)
{
    ref_instance = Referee_Register(&ref_config);
    if (ref_instance == NULL)
    {
        Log_Error("register referee failed");
    }
    while (1)
    {
        osDelay(1);
    }
}