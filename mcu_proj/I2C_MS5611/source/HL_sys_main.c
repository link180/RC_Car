
#include "MS5611.h"


int main(void)
{
    char txt_buf[256] = {0};
    unsigned int buf_len;
    long pressure =0;
    double temp =0;
   double altitude =0;



    sciInit();
    disp_set("SCI Configuration Success!!\n\r\0");


    i2cInit();
    wait(10000000);
    disp_set("I2C Init Success!!\n\r\0");

    Init_MS5611(MS5611_STANDARD);

    disp_set("Init MS5611 Success!!\n\r\0");

    for (;;)   // loop()
    {

        temp = readTemperature(true);
        pressure = readPressure(true);

        altitude = getAltitude(pressure, 101110); // 서울의 해수면 높이

        sprintf(txt_buf, "altitude = %lf \t pressure = %ld \t temperature = %lf \n\r\0",
                altitude,  pressure, temp );

        buf_len = strlen(txt_buf);
        sciDisplayText(sciREG1, (uint8 *) txt_buf, buf_len);
        wait(3000000);

    }

}
