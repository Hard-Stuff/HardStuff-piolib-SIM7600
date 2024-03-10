#pragma once

#define TINY_GSM_MODEM_SIM7600

#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 1024
#endif

// #define TINY_GSM_DEBUG Serial

#include <TinyGsm.h>
#include <TimeLib.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
#endif

#ifndef SIMCOM_BAUD
#define SIMCOM_BAUD 115200
#endif

#define SIM7600ClientSecure TinyGsmClientSecure
#define SIM7600Client TinyGsmClient

namespace SIMCOM
{
#pragma region DEFINES
#define SerialAT Serial1
#ifdef DUMP_AT_COMMANDS
    StreamDebugger debugger(SerialAT, Serial);
    TinyGsm modem(debugger);
#else
    TinyGsm modem(SerialAT);
#endif

#pragma endregion

#pragma region GPS_STRUCTS
#ifndef GPS_ACCURACY_LIMIT
#define GPS_ACCURACY_LIMIT 30000 // 30 metres
#endif
    struct GPSResponse
    {
        float lat = 0;
        float lon = 0;
        float speed = 0;
        float alt = 0;
        int vsat = 0;
        int usat = 0;
        float accuracy = 0;
        int year = 0;
        int month = 0;
        int day = 0;
        int hour = 0;
        int min = 0;
        int sec = 0;

        bool withinAccuracy() const { return (accuracy <= GPS_ACCURACY_LIMIT && accuracy != 0); }
        void print()
        {
            Serial.println("Latitude:" + String(lat, 8) + "\tLongitude:" + String(lon, 8));
            Serial.println("Speed:" + String(speed) + "\tAltitude:" + String(alt));
            Serial.println("Visible Satellites:" + String(vsat) + "\tUsed Satellites:" + String(usat));
            Serial.println("Accuracy:" + String(accuracy) + ", that is " + String(withinAccuracy() ? "" : "not ") + "within tolerance!");
            Serial.println("Year:" + String(year) + "\tMonth:" + String(month) + "\tDay:" + String(day));
            Serial.println("Hour:" + String(hour) + "\tMinute:" + String(min) + "\tSecond:" + String(sec));
        }
    };
#pragma endregion

    /**
     * @brief Get the GPS coordinates as a GPS response
     *
     * @param print_response_to_serial Optionally print the response to serial
     * @return GPSResponse
     */
    GPSResponse getGPSCoordinates()
    {
        GPSResponse gps_response;
        modem.getGPS(&gps_response.lat, &gps_response.lon, &gps_response.speed, &gps_response.alt, &gps_response.vsat, &gps_response.usat, &gps_response.accuracy,
                     &gps_response.year, &gps_response.month, &gps_response.day, &gps_response.hour, &gps_response.min, &gps_response.sec);
        return gps_response;
    }

    /**
     * @brief Get the time from the network cell tower
     * @returns true if successful, otherwise false
     */
    bool updateTime()
    {
        int year = 0;
        int month = 0;
        int day = 0;
        int hour = 0;
        int min = 0;
        int sec = 0;
        if (modem.getGPSTime(&year, &month, &day, &hour, &min, &sec))
        {
            if (year == 2023)
            {
                setTime(hour, min, sec, day, month, year);

                Serial.println("Time set from GPS");
                return true;
            }
        }

        float timezone;
        if (modem.getNetworkTime(&year, &month, &day, &hour, &min, &sec, &timezone))
        {
            if (year == 2023)
            {
                setTime(hour, min, sec, day, month, year);
                Serial.println("Time set from GSM Network");
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Initialize the SIMCOM module against the configs, and connect to network.
     *
     * @returns true if successful, otherwise false
     */
    bool init()
    {
        SerialAT.begin(SIMCOM_BAUD, SERIAL_8N1, SIMCOM_UART_RX, SIMCOM_UART_TX);
        delay(6000);

        // This can take quite some time
        Serial.println("Initializing modem... ");
        while (!modem.init())
        {
        }

        String modemInfo = modem.getModemInfo();
        Serial.print("Modem Info: ");
        Serial.println(modemInfo);

        Serial.print("Waiting for network... ");
        if (!modem.waitForNetwork())
        {
            Serial.println(" fail");
            return false;
        }

        if (modem.isNetworkConnected())
        {
            Serial.println("Network connected");
        }

        // Enable full functionality
        modem.setPhoneFunctionality(1);

        // Turn on the GPRS
        Serial.print(F("Connecting to "));
        Serial.print(SIMCOM_APN);
        if (!modem.gprsConnect(SIMCOM_APN))
        {
            Serial.println(" fail");
            return false;
        }
        Serial.println(" success");
        if (modem.isGprsConnected())
        {
            Serial.println("GPRS connected");
        }

        // // Update the internal clock (good for SSL)
        // modem.NTPServerSync();

        // Turn on the GPS
        modem.disableGPS();
        delay(500);
        if (modem.enableGPS())
        {
            Serial.println("GPS enabled");
        }
        return true;
    }

    String formatTimeISO8601(time_t t)
    {
        char buffer[25];

        // Break down time_t into its components
        int Year = year(t);
        int Month = month(t);
        int Day = day(t);
        int Hour = hour(t);
        int Minute = minute(t);
        int Second = second(t);

        // Format the string in ISO 8601 format
        // Note: This assumes UTC time. Adjust accordingly if using local time.
        snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d.000Z",
                 Year, Month, Day, Hour, Minute, Second);

        return String(buffer);
    }

    time_t formatTimeFromISO8601(String timestamp)
    {
        int Year, Month, Day, Hour, Minute, Second;
        sscanf(timestamp.c_str(), "%04d-%02d-%02dT%02d:%02d:%02d.000Z",
               &Year, &Month, &Day, &Hour, &Minute, &Second);
        tmElements_t tm;
        tm.Year = Year - 1970; // Adjust year
        tm.Month = Month;      // Adjust month
        tm.Day = Day;
        tm.Hour = Hour;
        tm.Minute = Minute;
        tm.Second = Second;
        return makeTime(tm); // Convert to time_t
    }
}