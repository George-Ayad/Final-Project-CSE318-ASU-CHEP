#include <stdint.h>
#include "tm4c123gh6pm.h"

/// Written by : omar zohdi - Abanoub Tarek - Fady Mina - George Guirgus ///


#define RLED            51
#define BLED            52
#define GLED            53
#define PushButton1     50
#define PushButton2     54

#define PA0     00
#define PA1     01
#define PA2     02
#define PA3     03
#define PA4     04
#define PA5     05
#define PA6     06
#define PA7     07

#define PB0     10
#define PB1     11
#define PB2     12
#define PB3     13
#define PB4     14
#define PB5     15
#define PB6     16
#define PB7     17

#define PC0     20
#define PC1     21
#define PC2     22
#define PC3     23
#define PC4     24
#define PC5     25
#define PC6     26
#define PC7     27


#define PD0     30
#define PD1     31
#define PD2     32
#define PD3     33
#define PD4     34
#define PD5     35
#define PD6     36
#define PD7     37

#define PE0     40
#define PE1     41
#define PE2     42
#define PE3     43
#define PE4     44
#define PE5     45
#define PE6     46
#define PE7     47

#define PF0     50
#define PF1     51
#define PF2     52
#define PF3     53
#define PF4     54


#define INPUT           1
#define OUTPUT          0
#define PULLUP          2
#define PULLDOWN        3

#define HIGH    1
#define LOW     0

#define SET_BIT(reg,bitN)       ((reg) |= (1<<bitN))
#define CLR_BIT(reg,bitN)       ((reg) &= ~(1<<bitN))
#define GET_BIT(reg,bitN)       (1 & ((reg)>>bitN))

void pinmode(uint8_t inPortPin, uint8_t dir);
void pinwrite(uint8_t inPortPin, uint8_t state);
uint8_t pinread(uint8_t inPortPin);