# BASIC PICO DATALOGGER WITH RTC

## How it works:

The trigger pin is ```GPIO 12``` which can be changed in the code to the pin of your preference. By default the pin is active high which can be changed in the code.
When pulled HIGH the logger starts logging the date and time the trigger was called and when pulled low after, records the stop date and time onto the sd card.

## Indicators:

The onboard LED is used to indicate various errors such as:

```SLOW BLINK(500ms) -> RTC ERROR```
```STEADY -> SD INIT ERROR```
```FAST BLINK(200ms) -> OPEN FILE ERROR```