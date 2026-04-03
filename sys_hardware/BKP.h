#ifndef __BKP_H
#define __BKP_H
#include "stdint.h"
void BKP_write(uint16_t addr,uint16_t data);

void tamper_init(uint16_t BKP_TamperPinLevel_Low_Or_Hight);

#endif // !__BKP_H
