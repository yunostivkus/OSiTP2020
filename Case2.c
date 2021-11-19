#include "mbed.h"
#include "Sht31.h"

Sht31   temp_sensor(I2C_SDA, I2C_SCL);
DigitalOut led(LED1);

int main()
{
    float h;
    int t = 1000;
    while (true) 
    {
        h = temp_sensor.readHumidity();
        printf(" %f\n\r", h);
        led = !led;
        if(h >= 65) 
        {
            t = 100;
            printf("Attention\n");
        }
        else
            t = 1000;
        wait_ms(t);
    }
}
