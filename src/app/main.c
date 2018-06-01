/*
 * main.c
 *
 *  Created on: 29 maj 2018
 *      Author: kamil
 */
#include <machine/hal.h>
#include <machine/_mx.h>
#include <uni/uni.h>
#include <uni/gpio.h>
#include <uni/pwrmode.h>
#include <breakpoint.h>
#include <uni/wdg.h>
#include <machine/rtc.h>
#include <time.h>
#include <uni/rtc.h>
#include <minmaxof.h>
#include <../../stm32cubemx/bluepill-pwrtest/Inc/main.h>

static const gpio_t gpio_led = GPIO_INIT(LED_GPIO_Port, LED_Pin);

time_t HW_RTC_ticks_to_time_t_Callback(RTC_HandleTypeDef *hrtc, time_t t)
{
	return t * 128 / LSE_VALUE;
}

time_t HW_RTC_time_t_to_ticks_Callback(RTC_HandleTypeDef *hrtc, time_t t)
{
	return t * LSE_VALUE / 128;
}

static inline void countdelay(unsigned long cnt)
{
	while(cnt--);
}

void blinkled(int times)
{
	for(int i = times + 1; i; --i) {
		gpio_set(gpio_led, 0);
		for (const clock_t stop = clock() + 200; clock() < stop; wdg_refresh());
		gpio_set(gpio_led, 1);
		for (const clock_t stop = clock() + 200; clock() < stop; wdg_refresh());
	}
}

int main(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_RCC_CLEAR_RESET_FLAGS();
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU | PWR_FLAG_SB);
	HAL_Init();
	void SystemClock_Config(void);
	SystemClock_Config();

	while(1);

	const long timeout = 2000;

	blinkled(1);
	for (const clock_t stop = clock() + timeout; clock() < stop; wdg_refresh());

	blinkled(2);
	for (const clock_t stop = clock() + timeout; clock() < stop; wdg_refresh()) {
		pwrmode_enter(PWRMODE_SLEEP);
	}

	blinkled(3);
	HW_RTC_alarm_set_ms(&hrtc, RTC_ALARM_A, timeout + 200);
	for (const time_ms_t stop = HW_RTC_time_ms(&hrtc) + timeout;
			HW_RTC_time_ms(&hrtc) < stop; wdg_refresh()) {
		pwrmode_enter(PWRMODE_STOP);
		SystemClock_Config();
	}

	blinkled(4);
	HW_RTC_alarm_set_ms(&hrtc, RTC_ALARM_A, timeout);
	pwrmode_enter(PWRMODE_STANDBY);
	for(;; wdg_refresh()) {
		breakpoint();
	}
}
