#include "thread.h"
#include "periph/gpio.h"
#include "xtimer.h"
#include "msg.h"
 
#define LED_BLUE GPIO_PIN(PORT_C, 8)
#define LED_GREEN GPIO_PIN(PORT_C, 9)
#define BUTTON GPIO_PIN(PORT_A,0)
#define BLINK_PERIOD 100000
 
char thread_one_stack[THREAD_STACKSIZE_DEFAULT];
char thread_two_stack[THREAD_STACKSIZE_DEFAULT];
 
int start_time = 0;
 
static kernel_pid_t thread_one_pid, thread_two_pid;
 
void blink_in_loop_blicking(int iterations, int period)
{
  xtimer_ticks32_t last_wakeup = xtimer_now();
  gpio_set(LED_BLUE);
  while(iterations--)
  {
    xtimer_periodic_wakeup(&last_wakeup, period);
    gpio_toggle(LED_BLUE);
    xtimer_periodic_wakeup(&last_wakeup, period);
    gpio_toggle(LED_BLUE);
  }
  gpio_set(LED_BLUE);
}
 

void btn_handler_one(void *arg)
{
  // Прием аргументов из главного потока
  (void)arg;
  // Создание объекта для хранения сообщения
  msg_t msg;
 
  static int last_handle = 0;
  static int amount_of_blinks = 0;
  int cur_handle = xtimer_usec_from_ticks(xtimer_now());
 
  if (cur_handle - last_handle > 100000)
  {
    msg.content.value = 1 + amount_of_blinks++ % 5;
      msg_send(&msg, thread_one_pid);
    last_handle = cur_handle;
  }
}
 
void btn_handler_two_rise(void* arg)
{
  (void)arg;
  start_time = xtimer_usec_from_ticks(xtimer_now());

}
 
void btn_handler_two_fall(void* arg)
{
  (void)arg;
 
  int cur_time = xtimer_usec_from_ticks(xtimer_now());
  msg_t msg;
 
  if (cur_time - start_time > 100000)
  {
    msg.content.value = (cur_time - start_time) / 100;
    msg_send(&msg, thread_two_pid);
  }
 
}
 
// Первый тред
void *thread_one(void *arg)
{
  (void) arg;
  msg_t msg;
  gpio_init(LED_BLUE, GPIO_OUT);
  while(1){
   
      msg_receive(&msg);
    blink_in_loop_blicking(msg.content.value, BLINK_PERIOD);
    // Переключаем светодиод
    gpio_toggle(GPIO_PIN(PORT_C,8));
  }    
  return NULL;
}
 
// Второй поток
void *thread_two(void *arg)
{
  (void) arg;
  gpio_init(LED_GREEN, GPIO_OUT);
  xtimer_ticks32_t last_wakeup2 = xtimer_now();
  int sleep_period = 333333;
  msg_t msg;
 
  while(1){
    xtimer_periodic_wakeup(&last_wakeup2, sleep_period);
    msg_receive(&msg);
    blink_in_loop_blicking(5, msg.content.value);
  }    
  return NULL;
}
 
 
int main(void)

  gpio_init_int(BUTTON, GPIO_IN, GPIO_RISING, btn_handler_two_rise, NULL);
  gpio_init_int(BUTTON, GPIO_IN, GPIO_FALLING, btn_handler_two_fall, NULL);

  thread_one_pid = thread_create(thread_one_stack, sizeof(thread_one_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  thread_one, NULL, "thread_one");

   thread_two_pid = thread_create(thread_two_stack, sizeof(thread_two_stack),
                  THREAD_PRIORITY_MAIN - 2, THREAD_CREATE_STACKTEST,
                  thread_two, NULL, "thread_two");
 
  while (1){
 
    }
 
    return 0;
}
 
/*
  Задание 1: Добавьте в код подавление дребезга кнопки
  Задание 2: Сделайте так, чтобы из прерывания по кнопке в поток thread_one передавалось целое число,
              которое означает, сколько раз должен моргнуть светодиод после нажатия кнопки.
              Передать значение в сообщении можно через поле msg.content.value
              После каждого нажатия циклически инкрементируйте значение от 1 до 5.
  Задание 3: Сделайте так, чтобы из прерывания по отпусканию кнопки в поток thread_two передавалось целое число, 
              которое задает значение интервала между морганиями светодиода на пине PC9.  
              Моргание светодиода не должно останавливаться. 
              После каждого нажатия циклически декрементируйте значение от 1000000 до 100000 с шагом 100000. 
              Чтобы послать сообщение асинхронно и без блокирования принимающего потока, нужно воспользоваться очередью.  
              Под очередь нужно выделить память в начале программы так: static msg_t rcv_queue[8]; 
              Затем в принимающем потоке нужно ее инициализировать так: msg_init_queue(rcv_queue, 8); 
              Поток может проверять, лежит ли что-то в очереди, это делается функцией msg_avail(),  
              которая возвращает количество элементов в очереди. 
