/*
File: Lab_6_JHB.c
Author: Josh Brake
Email: jbrake@hmc.edu
Date: 9/14/19
*/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

//Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Lab 6 Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Lab 6 Webpage</h1>";

char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";

char* resolutionStr = "<p>Temperature Sensor Resolution:</p>\
  <form action=\"res8\"><input type=\"submit\" value=\"Set 8-bit resolution\"></form>\
  <form action=\"res9\"><input type=\"submit\" value=\"Set 9-bit resolution\"></form>\
  <form action=\"res10\"><input type=\"submit\" value=\"Set 10-bit resolution\"></form>\
  <form action=\"res11\"><input type=\"submit\" value=\"Set 11-bit resolution\"></form>\
  <form action=\"res12\"><input type=\"submit\" value=\"Set 12-bit resolution\"></form>";

char* webpageEnd   = "</body></html>";

//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

int updateLEDStatus(char request[])
{
	int led_status = 0;
	// The request has been received. now process to determine whether to turn the LED on or off
	if (inString(request, "ledoff")==1) {
		digitalWrite(LED_PIN, PIO_LOW);
		led_status = 0;
	}
	else if (inString(request, "ledon")==1) {
		digitalWrite(LED_PIN, PIO_HIGH);
		led_status = 1;
	}

	return led_status;
}

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////

int updateResStatus(char request[]) {

	// send resolution setting command based on request
	if (inString(request, "res8") == 1) {
    return 0xE0;
  }
  else if (inString(request, "res9") == 1) {
    return 0xE2;
  }
  else if (inString(request, "res10") == 1) {
    return 0xE4;
  }
  else if (inString(request, "res11") == 1) {
    return 0xE6;
  }
  else if (inString(request, "res12") == 1) {
    return 0xE8;
	}

}

configurePins() {

  // set pin modes
  pinMode(PB1, GPIO_OUTPUT); // DS1722 CE pin
  pinMode(PB3, GPIO_ALT);    // SPI1 SCK
  pinMode(PB4, GPIO_ALT);    // SPI1 MISO
  pinMode(PB5, GPIO_ALT);    // SPI1 MOSI

  // Set alternate function to AF5 (SPI1) for PB3, PB4, PB5
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL3, 0b0101);
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 0b0101);
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 0b0101); 

}

int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  pinMode(PA6, GPIO_OUTPUT);
  
  RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
  initTIM(TIM15);

  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  configurePins();
  
  USART_TypeDef * USART = initUSART(USART1_ID, 125000);

  // TODO: Add SPI initialization code
  initSPI(0b011, 0, 1);

  while(1) {
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */

    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;
  
    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    }

    // TODO: Add SPI code here for reading temperature
    char resolution = updateResStatus(request);
    initTempSensor(resolution);
    double temperature = readTemp();
    char temperatureStr[32];
    sprintf(temperatureStr, "Temperature: %.4f deg C", temperature);
  
    // Update string with current LED state
  
    int led_status = updateLEDStatus(request);

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart);  // webpage header code
    sendString(USART, ledStr);        // button for controlling LED
    sendString(USART, resolutionStr); // buttons for setting resolution

    sendString(USART, "<h2>LED Status</h2>");

    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");


    sendString(USART, "<h2>Temperature Readings</h2>");

    sendString(USART, "<p>");
    sendString(USART, temperatureStr);
    sendString(USART, "</p>");

  
    sendString(USART, webpageEnd);
  }
}