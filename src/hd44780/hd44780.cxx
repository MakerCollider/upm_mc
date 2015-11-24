#include <iostream>
#include <unistd.h>
#include "hd44780.h"

using namespace upm;
using namespace std;
using namespace mraa;

HD44780::HD44780(uint8_t rs, uint8_t rw, uint8_t enable,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
            uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
    init(0, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

HD44780::HD44780(uint8_t rs, uint8_t enable,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
            uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
    init(0, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

HD44780::HD44780(uint8_t rs, uint8_t rw, uint8_t enable,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
    init(1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

HD44780::HD44780(uint8_t rs,  uint8_t enable,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
    init(1, rs, 255, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

void HD44780::init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
            uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
    _rs_pin = new mraa::Gpio(rs);
    _enable_pin = new mraa::Gpio(enable);

    if(rw == 255)
        _rw_pin = NULL;
    else
        _rw_pin = new mraa::Gpio(rw);

    if (fourbitmode)
    {
        _data_pins[0] = new mraa::Gpio(d0);
        _data_pins[1] = new mraa::Gpio(d1);
        _data_pins[2] = new mraa::Gpio(d2);
        _data_pins[3] = new mraa::Gpio(d3);
        _displayfunction = HD44780_4BITMODE | HD44780_1LINE | HD44780_5x8DOTS;
    }
    else
    {
        _data_pins[0] = new mraa::Gpio(d0);
        _data_pins[1] = new mraa::Gpio(d1);
        _data_pins[2] = new mraa::Gpio(d2);
        _data_pins[3] = new mraa::Gpio(d3);
        _data_pins[4] = new mraa::Gpio(d4);
        _data_pins[5] = new mraa::Gpio(d5);
        _data_pins[6] = new mraa::Gpio(d6);
        _data_pins[7] = new mraa::Gpio(d7);
        _displayfunction = HD44780_8BITMODE | HD44780_1LINE | HD44780_5x8DOTS;
    }
    begin(16, 1);  
}

void HD44780::begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
    if (lines > 1)
    {
        _displayfunction |= HD44780_2LINE;
    }
    _numlines = lines;

    setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);  

    // for some 1 line displays you can select a 10 pixel high font
    if ((dotsize != HD44780_5x8DOTS) && (lines == 1))
    {
        _displayfunction |= HD44780_5x10DOTS;
    }

    _rs_pin->dir(mraa::DIR_OUT);
    // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
    if (_rw_pin != NULL)
    { 
        _rw_pin->dir(mraa::DIR_OUT);
    }
    _enable_pin->dir(mraa::DIR_OUT);

    // Do these once, instead of every time a character is drawn for speed reasons.
    for (int i=0; i<((_displayfunction & HD44780_8BITMODE) ? 8 : 4); ++i)
    {
        _data_pins[i]->dir(mraa::DIR_OUT);
    } 

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
    usleep(50000); 
    // Now we pull both RS and R/W low to begin commands
    _rs_pin->write(0);
    _enable_pin->write(0);
    if (_rw_pin != NULL)
    { 
        _rw_pin->write(0);
    }

    //put the HD44780 into 4 bit or 8 bit mode
    if (! (_displayfunction & HD44780_8BITMODE))
    {
        // this is according to the hitachi HD44780 datasheet
        // figure 24, pg 46

        // we start in 8bit mode, try to set 4 bit mode
        write4bits(0x03);
        usleep(4500); // wait min 4.1ms

        // second try
        write4bits(0x03);
        usleep(4500); // wait min 4.1ms

        // third go!
        write4bits(0x03); 
        usleep(150);

        // finally, set to 4-bit interface
        write4bits(0x02); 
    }
    else
    {
        // this is according to the hitachi HD44780 datasheet
        // page 45 figure 23

        // Send function set command sequence
        command(HD44780_FUNCTIONSET | _displayfunction);
        usleep(4500);  // wait more than 4.1ms

        // second try
        command(HD44780_FUNCTIONSET | _displayfunction);
        usleep(150);

        // third go
        command(HD44780_FUNCTIONSET | _displayfunction);
    }

    // finally, set # lines, font size, etc.
    command(HD44780_FUNCTIONSET | _displayfunction);  

    // turn the display on with no cursor or blinking default
    _displaycontrol = HD44780_DISPLAYON | HD44780_CURSOROFF | HD44780_BLINKOFF;  
    display();

    // clear it off
    clear();

    // Initialize to default text direction (for romance languages)
    _displaymode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
    // set the entry mode
    command(HD44780_ENTRYMODESET | _displaymode);

}

void HD44780::setRowOffsets(int row0, int row1, int row2, int row3)
{
    _row_offsets[0] = row0;
    _row_offsets[1] = row1;
    _row_offsets[2] = row2;
    _row_offsets[3] = row3;
}

/********** high level commands, for the user! */
void HD44780::clear()
{
    command(HD44780_CLEARDISPLAY);  // clear display, set cursor position to zero
    usleep(2000);  // this command takes a long time!
}

void HD44780::home()
{
    command(HD44780_RETURNHOME);  // set cursor position to zero
    usleep(2000);  // this command takes a long time!
}

void HD44780::setCursor(uint8_t col, uint8_t row)
{
    const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
    if ( row >= max_lines )
    {
        row = max_lines - 1;    // we count rows starting w/0
    }
    if ( row >= _numlines )
    {
        row = _numlines - 1;    // we count rows starting w/0
    }

    command(HD44780_SETDDRAMADDR | (col + _row_offsets[row]));
}

// Turn the display on/off (quickly)
void HD44780::noDisplay()
{
    _displaycontrol &= ~HD44780_DISPLAYON;
    command(HD44780_DISPLAYCONTROL | _displaycontrol);
}
void HD44780::display()
{
    _displaycontrol |= HD44780_DISPLAYON;
    command(HD44780_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void HD44780::noCursor()
{
    _displaycontrol &= ~HD44780_CURSORON;
    command(HD44780_DISPLAYCONTROL | _displaycontrol);
}

void HD44780::cursor()
{
    _displaycontrol |= HD44780_CURSORON;
    command(HD44780_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void HD44780::noBlink()
{
    _displaycontrol &= ~HD44780_BLINKON;
    command(HD44780_DISPLAYCONTROL | _displaycontrol);
}

void HD44780::blink()
{
    _displaycontrol |= HD44780_BLINKON;
    command(HD44780_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void HD44780::scrollDisplayLeft(void)
{
    command(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void HD44780::scrollDisplayRight(void)
{
    command(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

// This is for text that flows Left to Right
void HD44780::leftToRight(void)
{
    _displaymode |= HD44780_ENTRYLEFT;
    command(HD44780_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void HD44780::rightToLeft(void)
{
    _displaymode &= ~HD44780_ENTRYLEFT;
    command(HD44780_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void HD44780::autoscroll(void)
{
    _displaymode |= HD44780_ENTRYSHIFTINCREMENT;
    command(HD44780_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void HD44780::noAutoscroll(void)
{
    _displaymode &= ~HD44780_ENTRYSHIFTINCREMENT;
    command(HD44780_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void HD44780::createChar(uint8_t location, uint8_t charmap[])
{
    location &= 0x7; // we only have 8 locations 0-7
    command(HD44780_SETCGRAMADDR | (location << 3));
    for (int i=0; i<8; i++)
    {
        write(charmap[i]);
    }
}

void HD44780::writeStr(std::string in_string)
{
    std::cout << in_string << std::endl;
    for(uint16_t i=0; i<in_string.length(); i++)
    {
        write(uint8_t(in_string[i]));
    }
}

/*********** mid level commands, for sending data/cmds */

void HD44780::command(uint8_t value)
{
    send(value, 0);
}

size_t HD44780::write(uint8_t value)
{
    send(value, 1);
    return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void HD44780::send(uint8_t value, uint8_t mode)
{
    _rs_pin->write(mode);

    // if there is a RW pin indicated, set it low to Write
    if (_rw_pin != NULL)
    { 
        _rw_pin->write(0);
    }

    if (_displayfunction & HD44780_8BITMODE)
    {
        write8bits(value); 
    }
    else
    {
        write4bits(value>>4);
        write4bits(value);
    }
}

void HD44780::pulseEnable(void)
{
    _enable_pin->write(0);
    usleep(1);    
    _enable_pin->write(1);
    usleep(1);    // enable pulse must be >450ns
    _enable_pin->write(0);
    usleep(100);   // commands need > 37us to settle
}

void HD44780::write4bits(uint8_t value)
{
    for (int i = 0; i < 4; i++)
    {
        _data_pins[i]->write((value >> i) & 0x01);
    }

    pulseEnable();
}

void HD44780::write8bits(uint8_t value)
{
    for (int i = 0; i < 8; i++)
    {
        _data_pins[i]->write((value >> i) & 0x01);
    }

    pulseEnable();
}