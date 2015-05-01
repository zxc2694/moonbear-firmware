04.29
自STM32F407板子移植FreeRTOS系統至STM32F429，出現以下問題：
/program/system/QuadCopterConfig.h
#define configRemoteControl 0
明明有遙控器卻必須設定為0才能運作

05.01
/program/system/QuadCopterConfig.h
/*motot PWM parameter adjustment*/
#define adjustmentMotor1 0        //100
