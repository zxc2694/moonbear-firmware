#include "QuadCopterConfig.h"

vs16 Tmp_PID_KP;
vs16 Tmp_PID_KI;
vs16 Tmp_PID_KD;
vs16 Tmp_PID_Pitch;

static __IO uint16_t pwm1_previous_value = 0;
static __IO uint16_t pwm2_previous_value = 0;
static __IO uint32_t pwm3_previous_value = 0;
static __IO uint32_t pwm4_previous_value = 0;
static __IO uint32_t pwm5_previous_value = 0;
static __IO uint8_t pwm1_is_rising = 1;
static __IO uint8_t pwm2_is_rising = 1;
static __IO uint8_t pwm3_is_rising = 1;
static __IO uint8_t pwm4_is_rising = 1;
static __IO uint8_t pwm5_is_rising = 1;


void TIM2_IRQHandler(void)
{
	uint32_t current[3];
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);

	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET) {
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

		if (pwm3_is_rising) {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;

			/* Get the Input Capture value */
			pwm3_previous_value = TIM_GetCapture1(TIM2);
			pwm3_is_rising = 0;

		} else {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

			//Get the Input Capture value
			current[0] =  TIM_GetCapture1(TIM2);

			if (current[0] > pwm3_previous_value)
				system.variable[PWM3_CCR].value =  current[0] - pwm3_previous_value;
			else if (current[0] < pwm3_previous_value)
				system.variable[PWM3_CCR].value = 0xFFFF - pwm3_previous_value + current[0] ;

			pwm3_is_rising = 1;
		}

		TIM_ICInit(TIM2, &TIM_ICInitStructure);

	}

	if (TIM_GetITStatus(TIM2, TIM_IT_CC2) == SET) {
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

		if (pwm4_is_rising) {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;

			/* Get the Input Capture value */
			pwm4_previous_value = TIM_GetCapture2(TIM2);
			pwm4_is_rising = 0;

		} else {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

			/* Get the Input Capture value */
			current[1] =  TIM_GetCapture2(TIM2);

			if (current[1] > pwm4_previous_value)
				system.variable[PWM4_CCR].value =  current[1] - pwm4_previous_value;
			else if (current[1] < pwm4_previous_value)
				system.variable[PWM4_CCR].value = 0xFFFF - pwm4_previous_value + current[1] ;

			pwm4_is_rising = 1;


		}

		TIM_ICInit(TIM2, &TIM_ICInitStructure);
	}

	if (TIM_GetITStatus(TIM2, TIM_IT_CC3) == SET) {
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);

		if (pwm5_is_rising) {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;

			/* Get the Input Capture value */
			pwm5_previous_value = TIM_GetCapture3(TIM2);
			pwm5_is_rising = 0;

		} else {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

			/* Get the Input Capture value */
			current[2] =  TIM_GetCapture3(TIM2);

			if (current[2] > pwm5_previous_value)
				system.variable[PWM5_CCR].value =  current[2] - pwm5_previous_value;
			else if (current[2] < pwm5_previous_value)
				system.variable[PWM5_CCR].value = 0xFFFF - pwm5_previous_value + current[2] ;

			pwm5_is_rising = 1;


		}

		TIM_ICInit(TIM2, &TIM_ICInitStructure);
	}
}

void TIM4_IRQHandler(void)
{
	uint16_t current[2];
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);

	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) == SET) {
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);

		if (pwm1_is_rising) {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;

			/* Get the Input Capture value */
			pwm1_previous_value = TIM_GetCapture1(TIM4);
			pwm1_is_rising = 0;

		} else {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

			/* Get the Input Capture value */
			current[0] =  TIM_GetCapture1(TIM4);

			if (current[0] > pwm1_previous_value)
				system.variable[PWM1_CCR].value =  current[0] - pwm1_previous_value;
			else if (current[0] < pwm1_previous_value)
				system.variable[PWM1_CCR].value = 0xFFFF - pwm1_previous_value + current[0] ;

			pwm1_is_rising = 1;
		}

		TIM_ICInit(TIM4, &TIM_ICInitStructure);

	}

	if (TIM_GetITStatus(TIM4, TIM_IT_CC2) == SET) {
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);

		if (pwm2_is_rising) {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;

			/* Get the Input Capture value */
			pwm2_previous_value = TIM_GetCapture2(TIM4);
			pwm2_is_rising = 0;

		} else {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

			/* Get the Input Capture value */
			current[1] =  TIM_GetCapture2(TIM4);

			if (current[1] > pwm2_previous_value)
				system.variable[PWM2_CCR].value =  current[1] - pwm2_previous_value;
			else if (current[1] < pwm2_previous_value)
				system.variable[PWM2_CCR].value = 0xFFFF - pwm2_previous_value + current[1] ;

			pwm2_is_rising = 1;


		}

		TIM_ICInit(TIM4, &TIM_ICInitStructure);
	}
}

void DMA1_Stream0_IRQHandler(void)
{
	I2C_RX_DMA_IRQ();
}

void DMA1_Stream6_IRQHandler(void)
{
	I2C_TX_DMA_IRQ();
}

void HardFault_Handler(void)
{
	while (1);
}
void MemManage_Handler(void)
{
	while (1);
}
void BusFault_Handler(void)
{
	while (1);
}
void UsageFault_Handler(void)
{
	while (1);
}
void DebugMon_Handler(void)
{
	while (1);
}
void NMI_Handler(void)
{
	while (1);
}

void SDIO_IRQHandler(void)
{
	long lHigherPriorityTaskWoken = pdFALSE;
	SD_ProcessIRQSrc();
	portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);
}

void DMA2_Stream3_IRQHandler(void)
{
	SD_ProcessDMAIRQ();
}
