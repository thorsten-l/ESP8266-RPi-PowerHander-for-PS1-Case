/* 
 * File:   main.cpp
 * Author: pi
 *
 * Created on April 6, 2018, 7:05 AM
 */

#include <cstdlib>
#include "MyGPIO.hpp"
#include <stdio.h>
#include <unistd.h>

using namespace std;

#define RPI_OFF_PIN       4
#define RPI_SHUTDOWN_PIN  17
#define RPI_REBOOT_PIN    27

/*
 * 
 */
int main(int argc, char** argv)
{
  gpio.pinMode(RPI_OFF_PIN, OUTPUT);
  gpio.pinMode(RPI_SHUTDOWN_PIN, INPUT_PULLUP);
  gpio.pinMode(RPI_REBOOT_PIN, INPUT_PULLUP);
  
  puts("\n\nRPi Power Agent for RPi Power Handler");
  puts("Version 0.0.2 by Dr. Thorsten Ludewig <t.ludewig@gmail.com>");
  puts( "Build date: " __DATE__ " " __TIME__  );
  fflush(stdout);
  
  gpio.digitalWrite( RPI_OFF_PIN, false );
  
  while( true )
  {
    if ( gpio.digitalRead(RPI_SHUTDOWN_PIN) == false )
    {
      puts( "RPi Power Agent: shutting down the system");
      fflush(stdout);
      system("/sbin/shutdown -h -P now");
    }
    
    if ( gpio.digitalRead(RPI_REBOOT_PIN) == false )
    {
      puts( "RPi Power Agent: rebooting the system");
      fflush(stdout);
      system("/sbin/shutdown -r now");
    }
    
    sleep(2);
  }
  return 0;
}

