#include "Wire.h"
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

#define TRIG 13

RTC_DS1307 rtc;
File logFile;

DateTime now;

bool trigger = false,
     isOpen = false,
     hasWritten = false; // check if file is open

void setup()
{
    Wire.setSDA(0);
    Wire.setSCL(1);

    pinMode(LED_BUILTIN, OUTPUT);

    while (!rtc.begin())
    {
        Serial.println("Couldn't connect to rtc!"); // write to sd
        digitalWrite(LED_BUILTIN, 1);
        delay(500);
        digitalWrite(LED_BUILTIN, 0);
        delay(500);
    }
    if (!rtc.isrunning())
    {
        Serial.println("Time needs to be set!");
        Serial.println("Enter \"sys\" to set the system date and time, send anything to set date and time manually.");
        while (!Serial.available())
            ;
        String data = Serial.readString();
        if (data.equalsIgnoreCase("sys"))
            rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        else
        {
            userInput(data);
        }
    }
    while (!SD.begin(4))
    {
        Serial.println("SD initialization failed!");
        digitalWrite(LED_BUILTIN, 1);
        delay(2000);
    }
    digitalWrite(LED_BUILTIN, 0);

    attachInterrupt(TRIG, ISR, CHANGE);
}
void ISR()
{
    if (digitalRead(TRIG))
        trigger = true;
    else
        trigger = false;
}
// void blinkLed(int delay)
// {
// }

void userInput(String data)
{
    Serial.println("Enter the date and time, format:(YY MM DD Hour Minute Second) in numericals");
    while (!Serial.available() > 0)
        ;
    data = Serial.readString();
    String dateNtime[6], field_val = "";
    for (int i = 0, field = 0; i <= data.length(); i++)
    {
        if (data[i] != ' ' && i != data.length())
            field_val += data[i];
        else
        {
            dateNtime[field++] = field_val;
            field_val = "";
            if (i == data.length() && field != 5)
            {
                Serial.println("Incorrect format! Retry.");
                userInput();
            }
        }
    }
    rtc.adjust(DateTime(dateNtime[0].toInt(), dateNtime[1].toInt(), dateNtime[2].toInt(), dateNtime[3].toInt(), dateNtime[4].toInt(), dateNtime[5].toInt()));
}

void loop()
{
    now = rtc.now();
    if (trigger && !hasWritten)
    {
        if (!isOpen)
        {
            openFile();
        }
        else
        {
            logFile.println("STARTED -> DATE: " + String(now.day()) + " TIME: " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
            logFile.close();
            hasWritten = true;
            isOpen = false;
        }
    }
    else if (!trigger && hasWritten)
    {
        if (!isOpen)
        {
            openFile();
        }
        else
        {
            logFile.println("STOPPED -> DATE: " + String(now.day()) + " TIME: " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
            logFile.close();
            hasWritten = false;
            isOpen = false;
        }
    }
}

void openFile()
{
    String filename = String(now.month()) + "-" + String(now.year()) + ".txt";
    logFile = SD.open(filename, FILE_WRITE);
    if (!logFile)
    {
        digitalWrite(LED_BUILTIN, 1);
        delay(200);
        digitalWrite(LED_BUILTIN, 0);
        delay(200);
    }
    else
        isOpen = true;
}