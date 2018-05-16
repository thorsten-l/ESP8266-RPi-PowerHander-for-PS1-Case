/* 
 * File:   MyGPIO.hpp
 * Author: pi
 *
 * Created on April 6, 2018, 7:07 AM
 */

#ifndef MYGPIO_HPP
#define MYGPIO_HPP

// FSELx - BCM2835 ARM Peripherals, page 92
typedef enum {
    INPUT = 0,
    OUTPUT = 1,
            
    ALTF4 = 3,
    ALTF5 = 2,
    ALTF0 = 4,
    ALTF1 = 5,
    ALTF2 = 6,
    ALTF3 = 7,
            
    // GPPUD - - BCM2835 ARM Peripherals, page 100
    INPUT_PULLUP = 8,
    INPUT_PULLDOWN = 9
} PINMODE;

class MyGPIO {
    
public:
    static MyGPIO& getInstance() {
        static MyGPIO singleton;
        return singleton;
    }

    void pinMode(int pin, PINMODE mode);
    void digitalWrite(int pin, bool value);
    bool digitalRead(int pin);

private:
    MyGPIO();
    virtual ~MyGPIO();
    volatile unsigned *gpioBase;

};

extern MyGPIO& gpio;

#endif /* MYGPIO_HPP */

