#ifndef GPIO_H
#define GPIO_H

void LEDON(void);
void LEDOFF(void);
unsigned long DETECT(void);
void gpio_init(void);
void led_init(void);
#endif
