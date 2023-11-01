#ifndef endtm1637
#define endtm1637

#include <Arduino.h>
#pragma once
/*
  Библиотека для 7-сегментного дисплея с десятичными точками
  на TM1637. Имеется:

   Смена яркости от 0 до 7 с возможностью отключить подсветку;
   Вывод "по байтам";
   Вывод отдельного символа;
   Вывод массива;
   Вывод int;
   Вывод бегущей строки;
   Вывод float с автоматическим и ручным определением кол-ва символов после запятой;
   Включение / выключение отдельных точек;
   Очистка дисплея.

  by EndPortal
  
  Версии
  v1.0 - релиз(14.12.2021)
*/

#define _LEFT 0
#define _RIGHT 1
#define _MIDDLE 2
#define _last 0b00000100
#define _dot 0b10000000
#define _empty 0b00000000
#define _0 0b00111111
#define _1 0b00000110
#define _2 0b01011011
#define _3 0b01001111
#define _4 0b01100110
#define _5 0b01101101
#define _6 0b01111101
#define _7 0b00000111
#define _8 0b01111111
#define _9 0b01101111
#define _A 0b01110111
#define _B 0b01111111
#define _b 0b01111100
#define _C 0b00111001
#define _c 0b01011000
#define _D 0b00111111
#define _d 0b01011110
#define _E 0b01111001
#define _e 0b01111011
#define _F 0b01110001
#define _G 0b00111101
#define _g 0b01101111
#define _H 0b01110110
#define _h 0b01110100
#define _i 0b00010000
#define _J 0b00011110
#define _L 0b00111000
#define _l 0b00000110
#define _n 0b01010100
#define _O 0b00111111
#define _o 0b01011100
#define _P 0b01110011
#define _q 0b01100111
#define _R 0b01110111
#define _r 0b01010000
#define _S 0b01101101
#define _t 0b01111000
#define _U 0b00111110
#define _u 0b00011100
#define _y 0b01101110
#define _under 0b00001000
#define _defis 0b01000000

#include <inttypes.h>
#include "fastIO.h"

const uint8_t digitToSegment[] = {
  //      A
  //     ---
  //  F |   | B
  //     -G-
  //  E |   | C
  //     ---
  //      D   X
  //XGFEDCBA
  0b00111111,    // 0
  0b00000110,    // 1
  0b01011011,    // 2
  0b01001111,    // 3
  0b01100110,    // 4
  0b01101101,    // 5
  0b01111101,    // 6
  0b00000111,    // 7
  0b01111111,    // 8
  0b01101111,    // 9
  0b01110111,    // A
  0b01111100,    // b
  0b00111001,    // C
  0b01011110,    // d
  0b01111001,    // E
  0b01110001     // F
};

class EndTM1637 {
  public:

    EndTM1637(byte ClK, byte DiO) {
      _ClK = ClK;
      _DiO = DiO;
      fastMode(_ClK, INPUT);
      fastMode(_DiO, INPUT);
      fastWrite(_ClK, LOW);
      fastWrite(_DiO, LOW);
      m_brightness = (7 & 0x7) | 0x08;
    }

    void setBright(byte brightness, bool on = 1) {
      m_brightness = (brightness & 0x7) | (on ? 0x08 : 0x00);
      setSegments(_lastbytes, 4, 0);
    }

    void setBytes(byte byte1, byte byte2, byte byte3, byte byte4) {
      byte hihg[4] = {byte1, byte2, byte3, byte4};
      for (byte i = 0; i < 4; i++) {
        if (hihg[i] == _last) hihg[i] = _lastbytes[i];
      }
      for (byte i = 0; i < 4; i++) {
        _lastbytes[i] = hihg[i];
      }
      setSegments(hihg, 4, 0);
    }

    void clear() {
      setBytes(0, 0, 0, 0);
    }

    void dots(int numpoint, bool stat = 1) {
      if (numpoint != -1) {
          bitWrite(_lastbytes[numpoint], 7, stat);
      } else {
        for (byte i = 0; i < 4; i++) {
          _lastbytes[i] &= ~bit(7);
        }
      }
      setBytes(_lastbytes[0], _lastbytes[1], _lastbytes[2], _lastbytes[3]);
    }

    void displayInts(int numberhi, bool nulles = 0, byte dotes = -1) {
      showNumberBaseEx(numberhi < 0 ? -10 : 10, numberhi < 0 ? -numberhi : numberhi, dotes, nulles, 4, 0);
    }

    void autoFloat(float data) {
	  if (data < 1.000 && data > -1.000) _nul = 1;
      else _nul = 0;
      uint8_t base = 0;
      if (data > 0 and floor(fabs(data)) < 10) {              // Если число без минуса и целая часть меньше 10 - можно вывести 3 дробных знака
        base = 3;
      } else if (floor(fabs(data)) < (data > 0 ? 100 : 10)) { // Если есть минус или же целая часть в пределах 9 / 99 - выводим 2 знака
        base = 2;
      } else {                                                // Иначе выводим с 1 знаком, вылезания за пределы задетектит сама функция вывода float
        base = 1;
      }
      if (base == 1) {
        displayInts(data * 10, 0, 2);
      }
      if (base == 2) {
        displayInts(data * 100, 0, 1);
      }
      if (base == 3) {
        displayInts(data * 1000, 0, 0);
      }
    }

    void manualFloat(float num, byte droby = 1, bool nulls = 0) {
      if (num < 1.000 && num > -1.000) _nul = 1;
      else _nul = 0;
      if (droby == 1) {
        displayInts(num * 10, nulls, 2);
      }
      if (droby == 2) {
        displayInts(num * 100, nulls, 1);
      }
      if (droby == 3) {
        displayInts(num * 1000, nulls, 0);
      }
    }

    void runningString(uint8_t *str, uint8_t size, uint16_t ms) {
      uint8_t strBuf[size + 8];                  // Временный буфер для всей строки
      uint8_t strPtr = 0;
      uint32_t strTimer = millis();

      for (uint8_t i = 0; i < 4; i++) {               // Добавляем пустые символы в начало строки
        strBuf[i] = _empty;
      }
      for (uint8_t i = 0; i < size; i++) {            // Добавляем символы самой строки
        strBuf[i + 4] = str[i];
      }
      for (uint8_t i = size + 4; i < size + 8; i++) { // Добавляем пустые символы в конец
        strBuf[i] = _empty;
      }

      while (strPtr < size + 4) {
        if (millis() - strTimer >= ms) {
          strTimer = millis();
          setBytes(strBuf[strPtr], strBuf[strPtr + 1], strBuf[strPtr + 2], strBuf[strPtr + 3]);
          strPtr++;
        }
      }
      delay(ms);
      clear();
    }

    void setArray(uint8_t *str, uint8_t size, byte stor = _LEFT) {
      if (size == 2) {
        if (stor == _LEFT) setBytes(str[0], str[1], _last, _last);
        else if (stor == _RIGHT) setBytes(_last, _last, str[0], str[1]);
        else if (stor == _MIDDLE) setBytes(_last, str[0], str[1], _last);
      }
      else if (size == 3) {
        if (stor == _LEFT) setBytes(str[0], str[1], str[2], _last);
        else if (stor == _RIGHT) setBytes(_last, str[0], str[1], str[2]);
      } else if (size == 4) {
        setBytes(str[0], str[1], str[2], str[3]);
      }
    }

    void setByte(byte byt, byte pos) {
      byte hihg[4] = {_last, _last, _last, _last};
      hihg[pos] = byt;
      setBytes(hihg[0], hihg[1], hihg[2], hihg[3]);
    }

  private:
    byte _ClK;
    byte _DiO;
    byte m_brightness;
    byte _lastbytes[4];
    bool _nul;

    void showNumberBaseEx(int8_t base, uint16_t num, uint8_t dos, bool leading_zero, uint8_t length, uint8_t pos) {
      bool negative = false;
      if (base < 0) {
        base = -base;
        negative = true;
      }

      uint8_t digits[4];
      if (num == 0 && !leading_zero) {
        for (uint8_t i = 0; i < (length - 1); i++)
          digits[i] = 0;
        digits[length - 1] = encodeDigit(0);
      }
      else {
        for (int i = length - 1; i >= 0; --i) {
          uint8_t digit = num % base;
          if (digit == 0 && num == 0 && leading_zero == false)
            digits[i] = 0;
          else
            digits[i] = encodeDigit(digit);
          if (digit == 0 && num == 0 && negative) {
            digits[i] = 0b01000000;
            negative = false;
          }

          num /= base;
        }
      }

      for (byte i = 0; i < 4; i++) {
        _lastbytes[i] = digits[i];
      }
      if (_nul == 1) {
		  _nul = 0;
		  _lastbytes[2] = _0;
	  }
      dots(dos);
    }

    void bitDelay() {
      delayMicroseconds(75);
    }

    void start() {
      fastMode(_DiO, OUTPUT);
      bitDelay();
    }

    void stop() {
      fastMode(_DiO, OUTPUT);
      bitDelay();
      fastMode(_ClK, INPUT);
      bitDelay();
      fastMode(_DiO, INPUT);
      bitDelay();
    }

    bool writeByte(uint8_t b) {
      uint8_t data = b;

      // 8 Data Bits
      for (uint8_t i = 0; i < 8; i++) {
        // CLK low
        fastMode(_ClK, OUTPUT);
        bitDelay();

        // Set data bit
        if (data & 0x01)
          fastMode(_DiO, INPUT);
        else
          fastMode(_DiO, OUTPUT);

        bitDelay();

        // CLK high
        fastMode(_ClK, INPUT);
        bitDelay();
        data = data >> 1;
      }

      // Wait for acknowledge
      // CLK to zero
      fastMode(_ClK, OUTPUT);
      fastMode(_DiO, INPUT);
      bitDelay();

      // CLK to high
      fastMode(_ClK, INPUT);
      bitDelay();
      uint8_t ack = fastRead(_DiO);
      if (ack == 0)
        fastMode(_DiO, OUTPUT);


      bitDelay();
      fastMode(_ClK, OUTPUT);
      bitDelay();

      return ack;
    }

    void showDots(uint8_t dos, uint8_t* digits) {
      for (int i = 0; i < 4; ++i)
      {
        digits[i] |= (dos & 0x80);
        dos <<= 1;
      }
    }

    uint8_t encodeDigit(uint8_t digit) {
      return digitToSegment[digit & 0x0f];
    }

    void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos) {
      // Write COMM1
      start();
      writeByte(0x40);
      stop();

      // Write COMM2 + first digit address
      start();
      writeByte(0xC0 + (pos & 0x03));

      // Write the data bytes
      for (uint8_t k = 0; k < length; k++)
        writeByte(segments[k]);

      stop();

      // Write COMM3 + brightness
      start();
      writeByte(0x80 + (m_brightness & 0x0f));
      stop();
    }
};
#endif