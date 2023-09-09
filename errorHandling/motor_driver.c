#include <SPI.h>
// Define the slave select pin. In the case of D1 Mini Po, it's D8
#define SS_PIN D8
void setup()
{
    Serial.begin(115200);
    SPI.begin();
    // The DRV8301 has a 1MHZ max communication speed.
    SPI.setClockDivider(SPI_CLOCK_DIV16); // Set this accordingly
    digitalWrite(CS, HIGH);               // Set chip select high to disable the device comms
}
uint16_t readDRV8301()
{
    uint16_t data = 0;

    digitalWrite(SS_PIN, LOW); // Set SS Pin LOW to select the DRV8301
    // The first bit (1) signifies this is a read command.
    // The following 4 bits represent the register address (0010), so register #2
    // The remaining 11 bits are the data bits. Since this is a read command, they are set to 0 (we don't write and read at the same time usually)
    SPI.transfer16(0b1001000000000000); // Send command to read from a specific register

    digitalWrite(SS_PIN, HIGH); // Set SS Pin HIGH to deselect the DRV8301

    delayMicroseconds(25);

    digitalWrite(SS_PIN, LOW); // Set SS Pin LOW to select the DRV8301

    data = SPI.transfer16(0x00); // Read the data from DRV8301

    digitalWrite(SS_PIN, HIGH); // Set SS Pin HIGH to deselect the DRV8301

    delayMicroseconds(25);

    return data;
}
uint16_t writeDRV8301()
{
    uint16_t data = 0;

    // Set SS Pin LOW to select the DRV8301
    digitalWrite(SS_PIN, LOW);

    // Send command to write to a specific register
    // The first bit is 0, indicating this is a write command
    // The next 4 bits is the register address (0010)
    // The remaining bits is the data
    SPI.transfer16(0b0001010000001010); // Replace with the data you want to send

    // Set SS Pin HIGH to deselect the DRV8301
    digitalWrite(SS_PIN, HIGH);

    delayMicroseconds(25);

    digitalWrite(SS_PIN, LOW); // Set SS Pin LOW to select the DRV8301

    data = SPI.transfer16(0x00); // Read the data from DRV8301

    digitalWrite(SS_PIN, HIGH); // Set SS Pin HIGH to deselect the DRV8301

    return data;
}