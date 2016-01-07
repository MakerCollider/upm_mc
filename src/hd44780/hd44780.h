#pragma once

#include <string>

#include "mraa.hpp"

namespace upm
{
    // commands
    #define HD44780_CLEARDISPLAY 0x01
    #define HD44780_RETURNHOME 0x02
    #define HD44780_ENTRYMODESET 0x04
    #define HD44780_DISPLAYCONTROL 0x08
    #define HD44780_CURSORSHIFT 0x10
    #define HD44780_FUNCTIONSET 0x20
    #define HD44780_SETCGRAMADDR 0x40
    #define HD44780_SETDDRAMADDR 0x80

    // flags for display entry mode
    #define HD44780_ENTRYRIGHT 0x00
    #define HD44780_ENTRYLEFT 0x02
    #define HD44780_ENTRYSHIFTINCREMENT 0x01
    #define HD44780_ENTRYSHIFTDECREMENT 0x00

    // flags for display on/off control
    #define HD44780_DISPLAYON 0x04
    #define HD44780_DISPLAYOFF 0x00
    #define HD44780_CURSORON 0x02
    #define HD44780_CURSOROFF 0x00
    #define HD44780_BLINKON 0x01
    #define HD44780_BLINKOFF 0x00

    // flags for display/cursor shift
    #define HD44780_DISPLAYMOVE 0x08
    #define HD44780_CURSORMOVE 0x00
    #define HD44780_MOVERIGHT 0x04
    #define HD44780_MOVELEFT 0x00

    // flags for function set
    #define HD44780_8BITMODE 0x10
    #define HD44780_4BITMODE 0x00
    #define HD44780_2LINE 0x08
    #define HD44780_1LINE 0x00
    #define HD44780_5x10DOTS 0x04
    #define HD44780_5x8DOTS 0x00

    class HD44780
    {
    public:
        HD44780(uint8_t rs, uint8_t enable,
                    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
        HD44780(uint8_t rs, uint8_t rw, uint8_t enable,
                    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
        HD44780(uint8_t rs, uint8_t rw, uint8_t enable,
                    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
        HD44780(uint8_t rs, uint8_t enable,
                    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

        void init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
                    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

        void begin(uint8_t cols, uint8_t rows, uint8_t charsize = HD44780_5x8DOTS);

        void clear();
        void home();

        void noDisplay();
        void display();
        void noBlink();
        void blink();
        void noCursor();
        void cursor();
        void scrollDisplayLeft();
        void scrollDisplayRight();
        void leftToRight();
        void rightToLeft();
        void autoscroll();
        void noAutoscroll();

        void setRowOffsets(int row1, int row2, int row3, int row4);
        void createChar(uint8_t, uint8_t[]);
        void setCursor(uint8_t, uint8_t); 
        virtual size_t write(uint8_t);
	void writeStr(std::string in_string);
        void command(uint8_t);
    private:
        void send(uint8_t, uint8_t);
        void write4bits(uint8_t);
        void write8bits(uint8_t);
        void pulseEnable();

        mraa::Gpio* _rs_pin; // LOW: command.  HIGH: character.
        mraa::Gpio* _rw_pin; // LOW: write to HD44780.  HIGH: read from HD44780.
        mraa::Gpio* _enable_pin; // activated by a HIGH pulse.
        mraa::Gpio* _data_pins[8];

        uint8_t _displayfunction;
        uint8_t _displaycontrol;
        uint8_t _displaymode;

        uint8_t _initialized;

        uint8_t _numlines;
        uint8_t _row_offsets[4];
    };
}