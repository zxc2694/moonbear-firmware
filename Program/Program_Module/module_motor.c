/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "module_motor.h"
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : Motor_Config
**功能 : 設定PWM輸出訊號
**輸入 : None
**輸出 : None
**使用 : Motor_Config();
**=====================================================================================================*/
/*=====================================================================================================*/
void Motor_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);




	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);

	/* TIM2 PWM3  PA0 */	/* TIM2 PWM4  PA1 */	/* TIM2 PWM5  PA2 */	/* TIM2 PWM8  PA3 */
	/* TIM3 PWM9  PA6 */	/* TIM3 PWM10 PA7 */
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	/* TIM3 PWM11 PB0 */	/* TIM3 PWM12 PB1 */	/* TIM4 PWM1  PB6 */	/* TIM4 PWM2  PB7 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);



	TIM_DeInit(TIM3);


	/************************** PWM Output **************************************/
	/* 設定 TIM2 TIM3 TIM4 Time Base */
	TIM_TimeBaseStruct.TIM_Period = (u16)(2500 - 1);            // 週期 = 2.5ms, 400kHz
	TIM_TimeBaseStruct.TIM_Prescaler = (u16)(84 - 1);           // 除頻84 = 1M ( 1us )
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;    // 上數
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);

	/* 設定 TIM2 TIM3 TIM4 TIM8 OC */
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;              // 配置為 PWM1 模式
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;  // 致能 OC
	TIM_OCInitStruct.TIM_Pulse = PWM_MOTOR_MAX;                 // 設置跳變值
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;      // 當計數值小於 PWM_MOTOR_MIN 時為高電平
	TIM_OC1Init(TIM3, &TIM_OCInitStruct);                       // 初始化 TIM3 OC1
	TIM_OC2Init(TIM3, &TIM_OCInitStruct);                       // 初始化 TIM3 OC2
	TIM_OC3Init(TIM3, &TIM_OCInitStruct);                       // 初始化 TIM3 OC3
	TIM_OC4Init(TIM3, &TIM_OCInitStruct);                       // 初始化 TIM3 OC4
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);           // 致能 TIM3 OC1 預裝載
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);           // 致能 TIM3 OC2 預裝載
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);           // 致能 TIM3 OC3 預裝載
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);           // 致能 TIM3 OC4 預裝載

	/* 啟動 */
	TIM_ARRPreloadConfig(TIM3, ENABLE);                         // 致能 TIM3 重載寄存器ARR
	TIM_Cmd(TIM3, ENABLE);                                      // 致能 TIM3

	PWM_Motor1 = PWM_MOTOR_MIN;
	PWM_Motor2 = PWM_MOTOR_MIN;
	PWM_Motor3 = PWM_MOTOR_MIN;
	PWM_Motor4 = PWM_MOTOR_MIN;
}
void PWM_Capture_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	uint16_t PrescalerValue = (uint16_t)((SystemCoreClock / 4) / 6000000) - 1;
	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM4, ENABLE);

	/* GPIOB clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);


	/* TIM2 PWM3  PA0 */  /* TIM2 PWM4  PA1 */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* TIM4 chennel2 configuration : PB.07 */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect TIM pin to AF2 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);

	/* Enable the TIM4 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(TIM2);
	TIM_DeInit(TIM4);
	TIM_TimeBaseStruct.TIM_Period = 0xFFFF;              // 週期 = 2.5ms, 400kHz
	TIM_TimeBaseStruct.TIM_Prescaler = 0;             // 除頻84 = 1M ( 1us )
	TIM_TimeBaseStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;    // 上數
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStruct);
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);
	TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);
	TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_ICInit(TIM2, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);

	/* Select the TIM4 Input Trigger: TI2FP2 */
	TIM_SelectInputTrigger(TIM4, TIM_TS_TI2FP2);
	TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2);
	/* Select the slave Mode: Reset Mode */
//  TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Reset);
//  TIM_SelectMasterSlaveMode(TIM4,TIM_MasterSlaveMode_Enable);

	/* TIM enable counter */
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	/* Enable the CC2 Interrupt Request */
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);
}

/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : Motor_Control
**功能 : 無刷馬達控制
**輸入 : Motor1, Motor2, Motor3, Motor4
**輸出 : None
**使用 : Motor_Control( Motor1, Motor2, Motor3, Motor4 );
**=====================================================================================================*/
/*=====================================================================================================*/
void Motor_Control(u16 Motor1, u16 Motor2, u16 Motor3, u16 Motor4)
{
	if (Motor1 > PWM_MOTOR_MAX)      Motor1 = PWM_MOTOR_MAX;
	else if (Motor1 < PWM_MOTOR_MIN) Motor1 = PWM_MOTOR_MIN;

	if (Motor2 > PWM_MOTOR_MAX)      Motor2 = PWM_MOTOR_MAX;
	else if (Motor2 < PWM_MOTOR_MIN) Motor2 = PWM_MOTOR_MIN;

	if (Motor3 > PWM_MOTOR_MAX)      Motor3 = PWM_MOTOR_MAX;
	else if (Motor3 < PWM_MOTOR_MIN) Motor3 = PWM_MOTOR_MIN;

	if (Motor4 > PWM_MOTOR_MAX)      Motor4 = PWM_MOTOR_MAX;
	else if (Motor4 < PWM_MOTOR_MIN) Motor4 = PWM_MOTOR_MIN;

	PWM_Motor1 = Motor1;
	PWM_Motor2 = Motor2;
	PWM_Motor3 = Motor3;
	PWM_Motor4 = Motor4;
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : Sevro_Control
**功能 : 伺服馬達控制
**輸入 : SevroA, SevroB
**輸出 : None
**使用 : Sevro_Control( SevroA, SevroB );
**=====================================================================================================*/
/*=====================================================================================================*/
void Sevro_Control(u16 SevroA, u16 SevroB)
{
//  if(SevroA>PWM_SEVRO_MAX)      SevroA = PWM_SEVRO_MAX;
//  else if(SevroA<PWM_SEVRO_MIN) SevroA = PWM_SEVRO_MIN;
//  if(SevroB>PWM_SEVRO_MAX)      SevroB = PWM_SEVRO_MAX;
//  else if(SevroB<PWM_SEVRO_MIN) SevroB = PWM_SEVRO_MIN;

//  PWM_SevroA = SevroA;
//  PWM_SevroB = SevroB;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
