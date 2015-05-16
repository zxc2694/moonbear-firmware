//馬達運作程式
//PWM 1  - PB 6 : roll 				//PWM 9  - PA 6 : 馬達1
//PWM 2  - PB 7 : pitch 				//PWM 10 - PA 7 : 馬達2
//PWM 3  - PA 0 : 推力 (throttle)			//PWM 11 - PB 0 : 馬達3
//PWM 4  - PA 1 : yaw				//PWM 12 - PB 1 : 馬達4
//PWM 5  - PA 2 : 安全開關

#include "QuadCopterConfig.h"

/* TIM2 PWM3  PA0 */	/* TIM2 PWM4  PA1 */	/* TIM2 PWM5  PA2 */	/* TIM2 PWM8  PA3 */
/* TIM3 PWM9  PA6 */	/* TIM3 PWM10 PA7 */

/* TIM3 PWM11 PB0 */	/* TIM3 PWM12 PB1 */	/* TIM4 PWM1  PB6 */	/* TIM4 PWM2  PB7 */
void Motor_Config(void)
{
	/* -- RCC Clock Configuration ----------------------------------------------- */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
	//GPIO使用之宣告，來自stm32f4xx_rcc.c，使用腳位PB、PA系列
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

	/* -- GPIO Configuration ---------------------------------------------------- */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);

	GPIO_InitTypeDef GPIO_InitStruct = {
		.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_UP
	};

	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* -- Timer Configuration --------------------------------------------------- */
	TIM_DeInit(TIM3);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct = {
		.TIM_Period = 2500 - 1,   //2.5ms , 400kHz
		.TIM_Prescaler = 84 - 1,  //84 = 1M(1us)
		.TIM_ClockDivision = TIM_CKD_DIV1,
		.TIM_CounterMode = TIM_CounterMode_Up
	};

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);


	/*TIM2 TIM3 TIM4 TIM8 */
	TIM_OCInitTypeDef TIM_OCInitStruct = {
		.TIM_OCMode = TIM_OCMode_PWM1,
		.TIM_OutputState = TIM_OutputState_Enable,
		.TIM_Pulse = PWM_MOTOR_MAX,
		.TIM_OCPolarity = TIM_OCPolarity_High
	};

	TIM_OC1Init(TIM3, &TIM_OCInitStruct);
	TIM_OC2Init(TIM3, &TIM_OCInitStruct);
	TIM_OC3Init(TIM3, &TIM_OCInitStruct);
	TIM_OC4Init(TIM3, &TIM_OCInitStruct);

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

	PWM_Motor1 = PWM_MOTOR_MIN;
	PWM_Motor2 = PWM_MOTOR_MIN;
	PWM_Motor3 = PWM_MOTOR_MIN;
	PWM_Motor4 = PWM_MOTOR_MIN;
}

void Motor_Control(u16 motor1, u16 motor2, u16 motor3, u16 motor4)
{	// 控制馬達之PWM使其介於PWM_MOTOR_MAX和PWM_MOTOR_MIN中
	
	if (motor1 > PWM_MOTOR_MAX)      motor1 = PWM_MOTOR_MAX;
	else if (motor1 < PWM_MOTOR_MIN) motor1 = PWM_MOTOR_MIN;

	if (motor2 > PWM_MOTOR_MAX)      motor2 = PWM_MOTOR_MAX;
	else if (motor2 < PWM_MOTOR_MIN) motor2 = PWM_MOTOR_MIN;

	if (motor3 > PWM_MOTOR_MAX)      motor3 = PWM_MOTOR_MAX;
	else if (motor3 < PWM_MOTOR_MIN) motor3 = PWM_MOTOR_MIN;

	if (motor4 > PWM_MOTOR_MAX)      motor4 = PWM_MOTOR_MAX;
	else if (motor4 < PWM_MOTOR_MIN) motor4 = PWM_MOTOR_MIN;

	PWM_Motor1 = motor1;
	PWM_Motor2 = motor2;
	PWM_Motor3 = motor3;
	PWM_Motor4 = motor4;
}

void PWM_Capture_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	uint16_t PrescalerValue = (uint16_t)((SystemCoreClock / 4) / 1000000) - 1;
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
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

