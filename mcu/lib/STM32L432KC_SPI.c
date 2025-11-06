// STM32L432KC_SPI.c
// Georgia Tai
// ytai@g.hmc.edu
// Oct. 25, 2025
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#include "STM32L432KC_SPI.h"

void initSPI(int br, int cpol, int cpha){

    // Set serial clock baud rate
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br);

    // Set clock polarity (CPOL) and phase (CPHA)
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);

    // clear lsbfirst

    // Set MCU as master
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_MSTR, 1);

    // Set data frame format (DS[3:0]) to 8 bits
    SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111);

    // ssm and ssi?

    // Enable SS output for master mode
    SPI1->CR2 |= _VAL2FLD(SPI_CR2_SSOE, 1);

    // Set FIFO reception threshold to 8 bits
    SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 1);

    // Enable SPI after configuration
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_SPE, 1);

}

char spiSendReceive(char send){

    // Wait until TX buffer is empty
    while(!(SPI1->SR & SPI_SR_TXE));

    // Send data
    *((volatile char *)&SPI1->DR) = send;

    // Wait until RX buffer is not empty
    while(!(SPI1->SR & SPI_SR_RXNE));

    // Read and return received data
    return *((volatile char *)&SPI1->DR);
}