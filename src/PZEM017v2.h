/*
* Copyright (c) 2023 Chung Ta (IT Cinet https://www.cinet.asia/) @thelightings 
 * Copyright (c) 2020 Maxz Maxzerker (for PZEM-017) https://github.com/maxzerker
 * Copyright (c) 2019 Jakub Mandula https://github.com/mandulaj
*/


#ifndef PZEM017v2_H
#define PZEM017v2_H



#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/* Disable Software Serial completely by defining: */
// #define PZEM017v2_NO_SWSERIAL

#if (not defined(PZEM017v2_NO_SWSERIAL) && (defined(__AVR__) || defined(ESP8266)) && not defined(ESP32))
/* Software serial is only available for AVRs and ESP8266 */
#define PZEM017v2_SOFTSERIAL
#endif

#if defined(PZEM017v2_SOFTSERIAL)
#include <SoftwareSerial.h>
#endif


#define PZEM_DEFAULT_ADDR   0xF8 
#define PZEM_BAUD_RATE      9600


class PZEM017v2
{
public:
#if defined(PZEM017v2_SOFTSERIAL)
    /* This will be deprecated */
    [[deprecated("Replaced by PZEM017v2(SoftwareSerial& port, uint8_t addr). Please pass the SoftwareSerial object instead: PZEM017v2(RX, TX)=>PZEM017v2(SWSerial)")]]
    PZEM017v2(uint8_t receivePin, uint8_t transmitPin, uint8_t addr=PZEM_DEFAULT_ADDR);
    
    /* SoftwareSerial version calls begin method */
    /* Stream version doesn't */
    PZEM017v2(SoftwareSerial& port, uint8_t addr=PZEM_DEFAULT_ADDR);
    PZEM017v2(Stream& port, uint8_t addr=PZEM_DEFAULT_ADDR);

#endif

#if defined(ESP32)
    /* ESP32 Hardware serial interface requires the receive and transmit pin specified */
    PZEM017v2(HardwareSerial& port, uint8_t receivePin, uint8_t transmitPin, uint8_t addr=PZEM_DEFAULT_ADDR);
    
    // Deprecate passing pointer
    [[deprecated("Replaced by PZEM017v2(HardwareSerial& port, uint8_t receivePin, uint8_t transmitPin, uint8_t addr). Please pass a reference instead of a pointer: PZEM017v2(&SerialX)=>PZEM017v2(SerialX)")]]
    PZEM017v2(HardwareSerial* port, uint8_t receivePin, uint8_t transmitPin, uint8_t addr=PZEM_DEFAULT_ADDR) : PZEM017v2(*port, receivePin, transmitPin, addr) {};
#else
    PZEM017v2(HardwareSerial& port, uint8_t addr=PZEM_DEFAULT_ADDR);
    
    // Deprecate passing pointer
    [[deprecated("Replaced by PZEM017v2(HardwareSerial& port, uint8_t addr). Please pass a reference instead of a pointer: PZEM017v2(&SerialX)=>PZEM017v2(SerialX)")]]
    PZEM017v2(HardwareSerial* port, uint8_t addr=PZEM_DEFAULT_ADDR) : PZEM017v2(*port, addr) {};
#endif
    // Empty constructor for creating arrays
    PZEM017v2(){};

    ~PZEM017v2();


    float voltage();
    float current();
    float power();
    float energy();
    bool voltage_alarm_high(); 
    bool voltage_alarm_low();

    bool getParameters();
    bool setAddress(uint8_t addr);
    uint8_t getAddress();
    uint8_t readAddress(bool update = false);
    bool setSAddress(uint8_t addr);
    uint8_t getSAddress();
    uint8_t readSAddress(bool update = false);


    bool setVoltageAlarm_HV(uint16_t volts);
    bool getVoltageAlarm_HV();
    bool setVoltageAlarm_LV(uint16_t volts);
    bool getPowerAlarm_LV();

    bool resetEnergy();

    void search();

private:

    Stream* _serial; // Serial interface
    bool _isSoft;    // Is serial interface software

    // TODO: if we remove the Local SW serial handling, we can get rid of this trash
    #if defined(PZEM017v2_SOFTSERIAL)
    SoftwareSerial* localSWserial = nullptr; // Pointer to the Local SW serial object
    #endif

    uint8_t _addr;   // Device address
    uint8_t _saddr;   // Device address

    bool _isConnected; // Flag set on successful communication

    struct {
        float voltage;
        float current;
        float power;
        float energy;
        uint16_t alarms_hv;
        uint16_t alarms_lv;
    }  _currentValues; // Measured values

    struct {
        float HVAlarmVoltage;
        float LVAlarmVoltage;
        uint16_t address;
        uint16_t shunttype;
    }  _parameterValues; // Parameter values

    uint64_t _lastRead; // Last time values were updated



    void init(Stream* port, bool isSoft, uint8_t addr); // Init common to all constructors

    bool updateValues();    // Get most up to date values from device registers and cache them
    uint16_t receive(uint8_t *resp, uint16_t len); // Receive len bytes into a buffer

    bool sendCmd8(uint8_t cmd, uint16_t rAddr, uint16_t val, bool check=false, uint16_t slave_addr=0xFFFF); // Send 8 byte command

    void setCRC(uint8_t *buf, uint16_t len);           // Set the CRC for a buffer
    bool checkCRC(const uint8_t *buf, uint16_t len);   // Check CRC of buffer

    uint16_t CRC16(const uint8_t *data, uint16_t len); // Calculate CRC of buffer
};



// Define for setting debug level verbosity
//#define PZEM017v2_DEBUG 0

#ifdef PZEM017v2_DEBUG

// For configuring custom serial debug interface, default: Serial
#ifndef PZEM017v2_DEBUG_SERIAL
#define PZEM017v2_DEBUG_SERIAL Serial
#endif


// For debugging
extern HardwareSerial PZEM017v2_DEBUG_SERIAL;


// Debugging function;
void printBuf(uint8_t* buffer, uint16_t len) {
    for(uint16_t i = 0; i < len; i++){
        char temp[6];
        sprintf(temp, "%.2x ", buffer[i]);
        PZEM017v2_DEBUG_SERIAL.print(temp);

    }
    PZEM017v2_DEBUG_SERIAL.println();
}
    #define DEBUG(...) PZEM017v2_DEBUG_SERIAL.print(__VA_ARGS__)
    #define DEBUGLN(...) PZEM017v2_DEBUG_SERIAL.println(__VA_ARGS__)
    #define DEBUGBUF(buf, len) printBuf(buf, len)


#else
    // Debugging mode off, disable the macros
    #define DEBUG(...)
    #define DEBUGLN(...)
    #define DEBUGBUF(buf, len)
#endif



#endif // PZEM017v2_H
