//1 задача: удаление дребезга кнопки


#include <stdio.h>
#include "xtimer.h"
#include "timex.h"
#include "periph/gpio.h"

/* set interval to 0.1 second */
#define INTERVAL (1U * US_PER_SEC  )
void btn_handler(void *arg)
{
  // Прием аргументов, передаваемых из главного потока.
  (void)arg;
  int a = gpio_read(GPIO_PIN(PORT_C, 8));
  if (a == 0)
    gpio_write(GPIO_PIN(PORT_C, 8), 1);
  else if (a > 0)
    gpio_write(GPIO_PIN(PORT_C, 8), 0);
  xtimer_usleep(100000);
}


int main(void)
{
    gpio_init(GPIO_PIN(PORT_C,8), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_C,9), GPIO_OUT);
    gpio_init_int(GPIO_PIN(PORT_A, 0), GPIO_IN, GPIO_BOTH, btn_handler, NULL);
    gpio_init(GPIO_PIN(PORT_C, 8), GPIO_OUT);
    xtimer_ticks32_t last_wakeup = xtimer_now();

    while(1) {
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
        printf("slept until %" PRIu32 "\r\n", xtimer_usec_from_ticks(xtimer_now()));
        gpio_toggle(GPIO_PIN(PORT_C,8));
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL + 0.1);
        gpio_toggle(GPIO_PIN(PORT_C,9));
    }

    return 0;
}


//3 задача: создание 1 и 2 потока

#include <stdio.h>
#include "xtimer.h"
#include "timex.h"
#include "periph/gpio.h"
char stack_one[THREAD_STACKSIZE_DEFAULT];
void *thread_one(void *arg)
{
    // Прием аргументов из главного потока
    (void) arg;
    // ВременнАя метка для отсчета времени сна
    xtimer_ticks32_t last_wakeup_one = xtimer_now();
    while(1){
        // Переключение состояния пина PC8
        gpio_toggle(GPIO_PIN(PORT_C,8));
        // Поток засыпает на 100000 микросекунд
        xtimer_periodic_wakeup(&last_wakeup_one, 100000);
    }
    return NULL;
}
void *thread_two(void *arg)
{
    // Прием аргументов из главного потока
    (void) arg;
    // ВременнАя метка для отсчета времени сна
    xtimer_ticks32_t last_wakeup_one = xtimer_now();
    while(1){
        // Переключение состояния пина PC9
        gpio_toggle(GPIO_PIN(PORT_C,9));
        // Поток засыпает на 100000 микросекунд
        xtimer_periodic_wakeup(&last_wakeup_one, 100000);
    }
    return NULL;
}

/* set interval to 0.1 second */
#define INTERVAL (1U * US_PER_SEC  )
#define LED_GREEN (1 << 12) /* port D, pin 12 */
#define LED_BLUE (1 << 13) /* port D, pin 13 */
void btn_handler(void *arg)
{
  // Прием аргументов, передаваемых из главного потока.
  (void)arg;
  int a = gpio_read(GPIO_PIN(PORT_C, 8));
  if (a == 0)
    gpio_write(GPIO_PIN(PORT_C, 8), 1);
  else if (a > 0)
    gpio_write(GPIO_PIN(PORT_C, 8), 0);
  xtimer_usleep(100000);
}
static inline void switch_leds_off(void){
    LED_PORT->ODR = 0;
}
static inline void setup_leds(void){
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;    // разрешаем тактирование порта D (диоды)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;   // и порта A (кнопка)
 
    GPIOA->MODER &= ~GPIO_MODER_MODER0;  // укажем что пин кнопки - вход
    LED_PORT->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 ;    /// на диоды - выход
}




int main(void)
{
    gpio_init(GPIO_PIN(PORT_C,8), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_C,9), GPIO_OUT);
    gpio_init_int(GPIO_PIN(PORT_A, 0), GPIO_IN, GPIO_BOTH, btn_handler, NULL);
    xtimer_ticks32_t last_wakeup = xtimer_now();
     thread_create(stack_one, sizeof(stack_one),
                    THREAD_PRIORITY_MAIN-1,
                    THREAD_CREATE_STACKTEST,
                    thread_one,
                    NULL, "thread_one");
   thread_create(stack_one, sizeof(stack_one),
                    THREAD_PRIORITY_MAIN-1,
                    THREAD_CREATE_STACKTEST,
                    thread_one,
                    NULL, "thread_two");
  

    while(1) {
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
        printf("slept until %" PRIu32 "\r\n", xtimer_usec_from_ticks(xtimer_now()));
        gpio_toggle(GPIO_PIN(PORT_C,8));
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL + 0.1);
        gpio_toggle(GPIO_PIN(PORT_C,9));
        
      
    }

    return 0;
}


//2 задача: при нажатии светодиод - горит, при отпускании кнопки тухнет

#include <stdio.h>
#include "xtimer.h"
#include "timex.h"
#include "periph/gpio.h"

/* set interval to 0.1 second */
#define INTERVAL (1U * US_PER_SEC  )
#define LED_GREEN (1 << 12) /* port D, pin 12 */
#define LED_BLUE (1 << 13) /* port D, pin 13 */
void btn_handler(void *arg)
{
  // Прием аргументов, передаваемых из главного потока.
  (void)arg;
  int a = gpio_read(GPIO_PIN(PORT_C, 8));
  if (a == 0)
    gpio_write(GPIO_PIN(PORT_C, 8), 1);
  else if (a > 0)
    gpio_write(GPIO_PIN(PORT_C, 8), 0);
  xtimer_usleep(100000);
}
static inline void switch_leds_off(void){
    LED_PORT->ODR = 0;
}
static inline void setup_leds(void){
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;    // разрешаем тактирование порта D (диоды)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;   // и порта A (кнопка)
 
    GPIOA->MODER &= ~GPIO_MODER_MODER0;  // укажем что пин кнопки - вход
    LED_PORT->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 ;    /// на диоды - выход
}




int main(void)
{
    gpio_init(GPIO_PIN(PORT_C,8), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_C,9), GPIO_OUT);
    gpio_init_int(GPIO_PIN(PORT_A, 0), GPIO_IN, GPIO_BOTH, btn_handler, NULL);
    gpio_init(GPIO_PIN(PORT_C, 8), GPIO_OUT);
    xtimer_ticks32_t last_wakeup = xtimer_now();

    while(1) {
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
        printf("slept until %" PRIu32 "\r\n", xtimer_usec_from_ticks(xtimer_now()));
        gpio_toggle(GPIO_PIN(PORT_C,8));
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL + 0.1);
        gpio_toggle(GPIO_PIN(PORT_C,9));
         if (GPIOA->IDR & GPIO_IDR_10) { // если кнопка нажата (на нулевом пине есть потенциал)
 
          LED_PORT->ODR = LED_GREEN|LED_BLUE; // включаем диоды
 
      } else {
            switch_leds_off();   //гасим
 
      }
    }

    return 0;
}
