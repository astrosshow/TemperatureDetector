#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h> 

void ds18b20_init(void);
float ds18b20_read_temp(void);

#endif 