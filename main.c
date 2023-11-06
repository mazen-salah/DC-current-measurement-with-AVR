#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char voltage_counter, current_counter;
unsigned int voltage, current;
volatile unsigned long voltage_buffer, current_buffer;

// ADC Interrupt Handler
ISR(ADC_vect)
{
    if (ADMUX & 0x0F) // If ADC1 channel is selected
    {
        voltage_buffer += ADC; // Accumulate voltage values in the buffer
        ADMUX = (ADMUX & 0xF0) | 0; // Select ADC0 channel
        voltage_counter++; // Increase measurement counter
    }
    else
    {
        current_buffer += ADC; // Accumulate current values in the buffer
        ADMUX = (ADMUX & 0xF0) | 1; // Select ADC1 channel
        current_counter++; // Increase measurement counter
    }
}

// Functions for working with LCD
#define RS PD0
#define EN PD2

// Function for sending a command to the LCD
void lcd_com(unsigned char p)
{
    PORTD &= ~(1 << RS); // RS = 0 (write command)
    PORTD |= (1 << EN); // EN = 1 (start writing command to LCD)
    PORTD &= 0x0F;
    PORTD |= (p & 0xF0); // High nibble
    _delay_us(100);
    PORTD &= ~(1 << EN); // EN = 0 (end writing command to LCD)
    _delay_us(100);
    PORTD |= (1 << EN); // EN = 1 (start writing command to LCD)
    PORTD &= 0x0F;
    PORTD |= (p << 4); // Low nibble
    _delay_us(100);
    PORTD &= ~(1 << EN); // EN = 0 (end writing command to LCD)
    _delay_us(100);
}

// Function for sending data
void lcd_data(unsigned char p)
{
    PORTD |= (1 << RS) | (1 << EN); // RS = 1 (write data), EN = 1 (start writing command to LCD)
    PORTD &= 0x0F;
    PORTD |= (p & 0xF0); // High nibble
    _delay_us(100);
    PORTD &= ~(1 << EN); // EN = 0 (end writing command to LCD)
    _delay_us(100);
    PORTD |= (1 << EN); // EN = 1 (start writing command to LCD)
    PORTD &= 0x0F;
    PORTD |= (p << 4); // Low nibble
    _delay_us(100);
    PORTD &= ~(1 << EN); // EN = 0 (end writing command to LCD)
    _delay_us(100);
}

// Function for displaying a string on the LCD
void lcd_string(unsigned char command, char *string)
{
    lcd_com(0x0C); // Turn on the display, cursor not visible
    lcd_com(command); // Address of the character position
    // Display characters until the end of the string
    while (*string != '\0')
    {
        lcd_data(*string); // Display character
        string++; // Next character in the string
    }
}

// Function for initializing the LCD
void lcd_init(void)
{
    DDRD = 0xFF;
    PORTD = 0x00;
    _delay_ms(50); // Wait for the LCD module to be ready
    // Configure four-bit mode
    PORTD |= (1 << PD5);
    PORTD &= ~(1 << PD4);
    // Activate four-bit mode
    PORTD |= (1 << EN);
    PORTD &= ~(1 << EN);
    _delay_ms(5);
    lcd_com(0x28); // 4-bit bus, LCD - 2 lines
    lcd_com(0x08); // Turn off the display
    lcd_com(0x01); // Clear the display
    _delay_us(100);
    lcd_com(0x06); // Shift the cursor to the right
    lcd_com(0x0C); // Turn on the display, cursor not visible
}

int main(void)
{
    // ADC Setup
    ADMUX |= (1 << REFS1) | (1 << REFS0); // Internal reference voltage 2.56V
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADFR) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADIE); // Enable ADC, start conversion, continuous mode, prescaler 64 (ADC frequency 125kHz), enable ADC interrupt

    sei(); // Globally enable interrupts

    lcd_init(); // Initialize the LCD

    _delay_ms(25);
    lcd_string(0x80, "VOLTS  * AMPERES");
    lcd_string(0xC0, "  .    *  .     ");

    while (1)
    {
        // Calculate average voltage value
        if (voltage_counter == 250)
        {
            voltage = ((voltage_buffer * 256 * 11) / 1024) / voltage_counter;
            voltage_counter = 0; // Reset measurement counter
            voltage_buffer = 0; // Reset voltage value buffer
            lcd_com(0xC0);
            lcd_data((voltage / 1000 % 10) + '0');
            lcd_data((voltage / 100 % 10) + '0');
            lcd_com(0xC3);
            lcd_data((voltage / 10 % 10) + '0');
        }

        // Calculate average current value
        if (current_counter == 250)
        {
            current = ((current_buffer * 256 * 10) / 1024) / current_counter;
            current_counter = 0; // Reset measurement counter
            current_buffer = 0; // Reset current value buffer
            lcd_com(0xC9);
            lcd_data((current / 1000 % 10) + '0');
            lcd_com(0xCB);
            lcd_data((current / 100 % 10) + '0');
            lcd_data((current / 10 % 10) + '0');
        }
    }
}
