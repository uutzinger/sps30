/**
 * sps-30 Library Header file
 *
 * Copyright (c) January 2019, Paul van Haastrecht
 *
 * All rights reserved.
 * Will work with either UART or I2c communication.
 * The I2C link has a number of restrictions. See detailed document
 *
 * Development environment specifics:
 * Arduino IDE 1.9
 *
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 * Version 1.0 / January 2019
 * - Initial version by paulvha
 *
 * Version 1.2 / January 2019
 * - added force serial1 when TX = RX = 8
 * - added flag  INCLUDE_SOFTWARE_SERIAL to exclude software Serial
 *********************************************************************
*/
#ifndef SPS30_H
#define SPS30_H

/**
 * To EXCLUDE I2C communication, maybe for resource reasons,
 * comment out the line below.
 */
#define INCLUDE_I2C   1

/**
 * To EXCLUDE the serial communication, maybe for resource reasons
 * as you board does not have a seperate serial, comment out the line below
 */
#define INCLUDE_UART 1

/**
 * On some IDE / boards software Serial is not available
 * comment out line below in that case
 */
#define INCLUDE_SOFTWARE_SERIAL 1

/**
 *  Auto detect that some boards have low memory. (like Uno) */
#if defined (__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
#define SMALLFOOTPRINT 1
#if defined INCLUDE_UART
#undef INCLUDE_UART
#endif //INCLUDE_UART
#endif

#include "Arduino.h"                // Needed for Stream
#include "Wire.h"                   // for I2c
#include "printf.h"

#if defined INCLUDE_SOFTWARE_SERIAL
#include <SoftwareSerial.h>         // softserial
#endif

/**
 *  The communication it can be :
 *   I2C_COMMS              use I2C communication
 *   SOFTWARE_SERIAL        Arduino variants and ESP8266 (On ESP32 software Serial is NOT very stable)
 *   SERIALPORT             ONLY IF there is NO monitor attached
 *   SERIALPORT1            Arduino MEGA2560, Sparkfun ESP32 Thing : MUST define new pins as defaults are used for flash memory)
 *   SERIALPORT2            Arduino MEGA2560 and ESP32
 *   SERIALPORT3            Arduino MEGA2560 only for now
 *   NONE                   No port defined
 *
 * Softserial has been left in as an option, but as the SPS30 is only
 * working on 115K the connection will probably NOT work on any device.
 */
typedef enum serial_port {
    I2C_COMMS = 0,
    SOFTWARE_SERIAL = 1,
    SERIALPORT = 2,
    SERIALPORT1 = 3,
    SERIALPORT2 = 4,
    SERIALPORT3 = 5,
    NONE = 6
};

/* structure to return all values */
typedef struct sps_values
{
    float   MassPM1;        // Mass Concentration PM1.0 [μg/m3]
    float   MassPM2;        // Mass Concentration PM2.5 [μg/m3]
    float   MassPM4;        // Mass Concentration PM4.0 [μg/m3]
    float   MassPM10;       // Mass Concentration PM10 [μg/m3]
    float   NumPM0;         // Number Concentration PM0.5 [#/cm3]
    float   NumPM1;         // Number Concentration PM1.0 [#/cm3]
    float   NumPM2;         // Number Concentration PM2.5 [#/cm3]
    float   NumPM4;         // Number Concentration PM4.0 [#/cm3]
    float   NumPM10;        // Number Concentration PM4.0 [#/cm3]
    float   PartSize;       // Typical Particle Size [μm]
};

/* used to get single value */
#define v_MassPM1 1
#define v_MassPM2 2
#define v_MassPM4 3
#define v_MassPM10 4
#define v_NumPM0 5
#define v_NumPM1 6
#define v_NumPM2 7
#define v_NumPM4 8
#define v_NumPM10 9
#define v_PartSize 10

/* needed for conversion float IEE754 */
typedef union {
    byte array[4];
    float value;
} ByteToFloat;

/* needed for auto interval timing */
typedef union {
    byte array[4];
    uint32_t value;
} ByteToU32;

/*************************************************************/
/* error codes */
#define ERR_OK          0x00
#define ERR_DATALENGTH  0X01
#define ERR_UNKNOWNCMD  0x02
#define ERR_ACCESSRIGHT 0x03
#define ERR_PARAMETER   0x04
#define ERR_OUTOFRANGE  0x28
#define ERR_CMDSTATE    0x43
#define ERR_TIMEOUT     0x50
#define ERR_PROTOCOL    0x51

typedef struct Description {
    uint8_t code;
    char    desc[80];
};

/* Receive buffer length*/
#if defined SMALLFOOTPRINT
#define MAXRECVBUFLENGTH 100         // for light boards
#else
#define MAXRECVBUFLENGTH 255
#endif

/*************************************************************/
/* SERIAL COMMUNICATION INFORMATION */
#define SER_START_MEASUREMENT       0x00
#define SER_STOP_MEASUREMENT        0x01
#define SER_READ_MEASURED_VALUE     0x03
#define SER_START_FAN_CLEANING      0x56
#define SER_RESET                   0xD3

#define SER_READ_DEVICE_INFO        0xD0    // GENERIC device request
#define SER_READ_DEVICE_PRODUCT_NAME    0xF1
#define SER_READ_DEVICE_ARTICLE_CODE    0xF2
#define SER_READ_DEVICE_SERIAL_NUMBER   0xF3

#define SER_AUTO_CLEANING_INTERVAL  0x80    // Generic autoclean request
#define SER_READ_AUTO_CLEANING          0x81    // read autoclean
#define SER_WRITE_AUTO_CLEANING         0x82    // write autoclean

#define SHDLC_IND   0x7e                   // header & trailer
#define TIME_OUT    5000                   // timeout to prevent deadlock read
#define RX_DELAY_MS 200                    // wait between write and read

/*************************************************************/
/* I2C COMMUNICATION INFORMATION  */
#define I2C_START_MEASUREMENT       0x0010
#define I2C_STOP_MEASUREMENT        0x0104
#define I2C_READ_DATA_RDY_FLAG      0x0202
#define I2C_READ_MEASURED_VALUE     0x0300
#define I2C_AUTO_CLEANING_INTERVAL  0x8004
#define I2C_SET_AUTO_CLEANING_INTERVAL      0x8005
#define I2C_START_FAN_CLEANING      0x5607
#define I2C_READ_ARTICLE_CODE       0xD025
#define I2C_READ_SERIAL_NUMBER      0xD033
#define I2C_RESET                   0xD304

#define SPS30_ADDRESS 0x69                 // I2c address
/***************************************************************/

class SPS30
{
  public:

    SPS30(void);

    /**
     * @brief : set RX and TX pin for softserial and Serial1 on ESP32
     */
    void SetSerialPin(uint8_t rx, uint8_t tx);

    /**
    * @brief  Enable or disable the printing of sent/response HEX values.
    *
    * @param act : level of debug to set
    *  0 : no debug message
    *  1 : sending and receiving data
    *  2 : 1 +  protocol progress
    */
    void EnableDebugging(uint8_t act);

    /**
     * @brief Initialize the communication port
     *
     * @param port : communication channel to be used (see sps30.h)
     */
    bool begin(serial_port port = SERIALPORT2); //If user doesn't specify Serial2 will be used

    /**
     * @brief : Perform SPS-30 instructions
     */
    bool probe();
    bool reset() {return(Instruct(SER_RESET));}
    bool start() {return(Instruct(SER_START_MEASUREMENT));}
    bool stop() {return(Instruct(SER_START_MEASUREMENT));}
    bool clean() {return(Instruct(SER_START_FAN_CLEANING));}

    /**
     * @brief : Set or get Auto Clean interval
     */
    uint8_t GetAutoCleanInt(uint32_t *val);
    uint8_t SetAutoCleanInt(uint32_t val);

    /**
     * @brief : retrieve Error message details
     */
    void GetErrDescription(uint8_t code, char *buf, int len);

    /**
     * @brief : retrieve device information from the SPS-30
     */
    uint8_t GetSerialNumber(char *ser, uint8_t len) {return(Get_Device_info( SER_READ_DEVICE_SERIAL_NUMBER, ser, len));}
    uint8_t GetArticleCode(char *ser, uint8_t len)  {return(Get_Device_info( SER_READ_DEVICE_ARTICLE_CODE, ser, len));}
    uint8_t GetProductName(char *ser, uint8_t len)  {return(Get_Device_info(SER_READ_DEVICE_PRODUCT_NAME, ser, len));}

    /**
     * @brief : retrieve all measurement values from SPS-30
     */
    uint8_t GetValues(struct sps_values *v);

    /**
     * @brief : retrieve a specific value from the SPS-30
     */
    float GetMassPM1()  {return(Get_Single_Value(v_MassPM1));}
    float GetMassPM2()  {return(Get_Single_Value(v_MassPM2));}
    float GetMassPM4()  {return(Get_Single_Value(v_MassPM4));}
    float GetMassPM10() {return(Get_Single_Value(v_MassPM10));}
    float GetNumPM0()   {return(Get_Single_Value(v_NumPM0));}
    float GetNumPM1()   {return(Get_Single_Value(v_NumPM1));}
    float GetNumPM2()   {return(Get_Single_Value(v_NumPM2));}
    float GetNumPM4()   {return(Get_Single_Value(v_NumPM4));}
    float GetNumPM10()  {return(Get_Single_Value(v_NumPM10));}
    float GetPartSize() {return(Get_Single_Value(v_PartSize));}

  private:

    /** shared variables */
    uint8_t _Receive_BUF[MAXRECVBUFLENGTH]; // buffers
    uint8_t _Send_BUF[10];
    uint8_t _Receive_BUF_Length;
    uint8_t _Send_BUF_Length;
    serial_port _Sensor_Comms;  // comunication channel to use
    int _SPS30_Debug;           // program debug level
    bool _started;              // indicate the measurement has started
    uint8_t Reported[11];       // use as cache indicator single value

    /** shared supporting routines */
    uint8_t Get_Device_info(uint8_t type, char *ser, uint8_t len);
    bool Instruct(uint8_t type);
    float Get_Single_Value(uint8_t value);
    float byte_to_float(int x);
    uint32_t byte_to_U32(int x);
    uint8_t Serial_RX = 0, Serial_TX = 0; // softserial or Serial1 on ESP32

#if defined INCLUDE_UART
    /** UART / serial related */
    // calls
    bool setSerialSpeed();
    uint8_t ReadFromSerial();
    uint8_t SerialToBuffer();
    uint8_t SendToSerial();
    bool SHDLC_fill_buffer(uint8_t command, uint32_t parameter = 0);
    uint8_t SHDLC_calc_CRC(uint8_t * buf, uint8_t first, uint8_t last);
    int ByteStuff(uint8_t b, int off);
    uint8_t ByteUnStuff(uint8_t b);

    // variables
    Stream *_serial;        // serial port to use
#endif // INCLUDE_UART


#if defined INCLUDE_I2C
    /** I2C communication */
    void I2C_init();
    void I2C_fill_buffer(uint16_t cmd, uint32_t interval = 0);
    uint8_t I2C_ReadToBuffer(uint8_t count, bool chk_zero);
    uint8_t I2C_SetPointer_Read(uint8_t cnt, bool chk_zero = false);
    uint8_t I2C_SetPointer();
    bool I2C_Check_data_ready();
    uint8_t I2C_calc_CRC(uint8_t data[2]);
#endif // INCLUDE_I2C

};
#endif /* SPS30_H */
