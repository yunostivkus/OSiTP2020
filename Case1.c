#include "mbed.h"
 
DigitalIn mybutton(USER_BUTTON);
DigitalOut myled(LED1);

int main() 
{
    mybutton.mode(PullUp);
    while (1)  
    {
        if (mybutton == 0) 
        {
            for (int i = 0; i < 20; ++i)
            {
                myled = !myled; // Toggle the LED state
                wait_ms(500); // 500 ms
            }
        }
        else  
          myled = 1;
    }
    
}
