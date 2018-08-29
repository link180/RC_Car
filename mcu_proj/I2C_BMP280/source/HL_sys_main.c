
#include "BMP280.h"


int main(void)
{
    char txt_buf[256] = {0};
    unsigned int buf_len;
    float altitude, pressure, temperature;

    sciInit();
    disp_set("SCI Configuration Success!!\n\r\0");


    i2cInit();
    wait(10000000);
    disp_set("I2C Init Success!!\n\r\0");

    BMP280_Init();

    disp_set("BMP280 Init Success!!\n\r\0");
    init_altitude(1011.10);

    for (;;)   // loop()
    {
        altitude = bmp280_readAltitude(1011.10); // 서울의 해수면 높이
        pressure = bmp280_readPressure();
        temperature = bmp280_readTemperature();

        sprintf(txt_buf, "altitude = %f \t \n\r\0",
                 altitude);

        buf_len = strlen(txt_buf);
        sciDisplayText(sciREG1, (uint8 *) txt_buf, buf_len);
       // wait(3000000);

    }

}
