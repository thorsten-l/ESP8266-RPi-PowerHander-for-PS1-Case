/* 
 * File:   MyGPIO.cpp
 * Author: pi
 * 
 * Created on April 6, 2018, 7:07 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "MyGPIO.hpp"

#define BCM2708_PERI_BASE 0x3F000000
#define GPIO_BASE   (BCM2708_PERI_BASE + 0x200000)
#define BLOCK_SIZE  (4*1024)

#define GPIO_FSEL(PIN)      *(gpioBase+(PIN/10))
#define GPIO_SET            *(gpioBase+7)
#define GPIO_CLR            *(gpioBase+10)
#define GPIO_GPLEV(PIN)     *(gpioBase+13+(PIN/32))
#define GPIO_GPPUD          *(gpioBase+37)
#define GPIO_GPPUDCLK(PIN)  *(gpioBase+38+(PIN/32))

MyGPIO& gpio = MyGPIO::getInstance();

MyGPIO::MyGPIO()
{
  int mem_fd;
  void *gpio_map;

  if ((mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC)) < 0)
  {
    printf("can't open /dev/gpiomem \n");
    exit(-1);
  }

  gpio_map = mmap(
          NULL, // Any adddress in our space will do
          BLOCK_SIZE, // Map length
          PROT_READ | PROT_WRITE, // Enable reading & writting to mapped memory
          MAP_SHARED, // Shared with other processes
          mem_fd, // File to map
          GPIO_BASE // Offset to GPIO peripheral
          );

  close(mem_fd);

  if (gpio_map == MAP_FAILED)
  {
    printf("mmap error %d\n", (int) gpio_map);
    exit(-1);
  }

  gpioBase = (volatile unsigned *) gpio_map;
}

MyGPIO::~MyGPIO()
{
}

void MyGPIO::pinMode(int pin, PINMODE mode)
{
  // Clear all mode flags => INPUT mode
  GPIO_FSEL(pin) &= ~(7 << (((pin) % 10)*3));

  if (mode > INPUT && mode < INPUT_PULLUP)
  {
    GPIO_FSEL(pin) |= (mode << (((pin) % 10)*3));
  }

  if (mode == INPUT || mode == INPUT_PULLUP || mode == INPUT_PULLDOWN)
  {
    int pullMode = 0;

    if (mode > INPUT)
    {
      pullMode = (mode == INPUT_PULLDOWN) ? 1 : 2;
    }

    GPIO_GPPUD = pullMode;
    usleep(5);
    GPIO_GPPUDCLK(pin) = 1 << (pin % 32);
    usleep(5);

    GPIO_GPPUD = 0;
    usleep(5);
    GPIO_GPPUDCLK(pin) = 0;
    usleep(5);
  }
}

void MyGPIO::digitalWrite(int pin, bool value)
{
  if (value)
  {
    GPIO_SET = 1 << pin;
  }
  else
  {
    GPIO_CLR = 1 << pin;
  }
}

bool MyGPIO::digitalRead(int pin)
{
  return GPIO_GPLEV(pin) & (1<<(pin%32));
}
