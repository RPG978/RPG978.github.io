---
layout: default
---

# IoT Thermostat
## Artifact Summary

This project is an IoT thermostat prototype built on embedded hardware, designed to read environmental data, manage heating and cooling states, 
and communicate status updates as if connected to a cloud analytics platform. It integrates an AHT20 temperature sensor over I²C, uses LEDs and 
a 16×2 LCD for real‑time user feedback, and supports user interaction through mode and setpoint adjustment buttons. Although cloud connectivity 
was simulated via UART, the system was architected to eventually integrate with cloud‑based monitoring tools.

---

## Original Python Code
<details markdown="1">
<summary><strong>Click to view Python code</strong></summary>

```python
# SNHU - CS 350
# Thermostat - This is the Python code used to demonstrate
# the functionality of the thermostat that we have prototyped throughout
# the course. 
#
# This code works with the test circuit that was built for module 7.
#
# Functionality:
#
# The thermostat has three states: off, heat, cool
#
# The lights will represent the state that the thermostat is in.
#
# If the thermostat is set to off, the lights will both be off.
#
# If the thermostat is set to heat, the Red LED will be fading in 
# and out if the current temperature is below the set temperature;
# otherwise, the Red LED will be on solid.
#
# If the thermostat is set to cool, the Blue LED will be fading in 
# and out if the current temperature is above the set temperature;
# otherwise, the Blue LED will be on solid.
#
# One button will cycle through the three states of the thermostat.
#
# One button will raise the set point by a degree.
#
# One button will lower the set point by a degree.
#
# The LCD will display the date and time on one line and
# alternate the second line between the current temperature and 
# the state of the thermostat along with its set temperature.
#
# The Thermostat will send a status update to the TemperatureServer
# over the serial port every 30 seconds in a comma delimited string
# including the state of the thermostat, the current temperature
# in degrees Fahrenheit, and the set point of the thermostat.
#

# Import necessary to provide timing in the main loop
from time import sleep
from datetime import datetime

# Imports required to allow us to build a fully functional state machine
from statemachine import StateMachine, State

# Imports necessary to provide connectivity to the
# thermostat sensor and the I2C bus
import board
import adafruit_ahtx0

# These are the packages that we need to pull in so that we can work
# with the GPIO interface on the Raspberry Pi board and work with
# the 16x2 LCD

# import board - already imported for I2C connectivity
import digitalio
import adafruit_character_lcd.character_lcd as characterlcd

# This imports the Python serial package to handle communications over the
# Raspberry Pi's serial port.
import serial

# Imports required to handle our Button, and our PWMLED devices
from gpiozero import Button, PWMLED

# This package is necessary so that we can delegate the blinking
# lights to their own thread so that more work can be done at the
# same time
from threading import Thread

# This is needed to get coherent matching of temperatures.
from math import floor

# DEBUG flag - boolean value to indicate whether to print
# status messages on the console of the program
DEBUG = True

# Create an I2C instance so that we can communicate with
# devices on the I2C bus.
i2c = board.I2C()

# Initialize our Temperature and Humidity sensor
thSensor = adafruit_ahtx0.AHTx0(i2c)

# Initialize our serial connection

# Because we imported the entire package instead of just importing Serial and
# some of the other flags from the serial package, we need to reference those
# objects with dot notation.

# e.g. ser = serial.Serial
ser = serial.Serial(
    port='/dev/ttyS0',  # This would be /dev/ttyAM0 prior to Raspberry Pi 3
    baudrate=115200,  # This sets the speed of the serial interface in
    # bits/second
    parity=serial.PARITY_NONE,  # Disable parity
    stopbits=serial.STOPBITS_ONE,  # Serial protocol will use one stop bit
    bytesize=serial.EIGHTBITS,  # We are using 8-bit bytes
    timeout=1  # Configure a 1-second timeout
)

# Our two LEDs, utilizing GPIO 18, and GPIO 23
redLight = PWMLED(18)
blueLight = PWMLED(23)


# ManagedDisplay - Class intended to manage the 16x2
# Display

# This code is largely taken from the work done in module 4, and
# converted into a class so that we can more easily consume the
# operational capabilities.
class ManagedDisplay():
    
    # Class Initialization method to set up the display
    def __init__(self):

        # Set up the six GPIO lines to communicate with the display.
        # This leverages the digitalio class to handle digital
        # outputs on the GPIO lines. There is also an analogous
        # class for analog IO.

        # You need to make sure that the port mappings match the
        # physical wiring of the display interface to the
        # GPIO interface.

        # compatible with all versions of RPI as of Jan. 2019

        self.lcd_rs = digitalio.DigitalInOut(board.D17)
        self.lcd_en = digitalio.DigitalInOut(board.D27)
        self.lcd_d4 = digitalio.DigitalInOut(board.D5)
        self.lcd_d5 = digitalio.DigitalInOut(board.D6)
        self.lcd_d6 = digitalio.DigitalInOut(board.D13)
        self.lcd_d7 = digitalio.DigitalInOut(board.D26)

        # Modify this if you have a different sized character LCD
        self.lcd_columns = 16
        self.lcd_rows = 2

        # Initialise the lcd class
        self.lcd = characterlcd.Character_LCD_Mono(self.lcd_rs, self.lcd_en,
                                                   self.lcd_d4, self.lcd_d5, self.lcd_d6, self.lcd_d7,
                                                   self.lcd_columns, self.lcd_rows)

        # wipe LCD screen before we start
        self.lcd.clear()

    # cleanupDisplay - Method used to clean up the digitalIO lines that
    # are used to run the display.
    def cleanupDisplay(self):
        # Clear the LCD first - otherwise we won't be abe to update it.
        self.lcd.clear()
        self.lcd_rs.deinit()
        self.lcd_en.deinit()
        self.lcd_d4.deinit()
        self.lcd_d5.deinit()
        self.lcd_d6.deinit()
        self.lcd_d7.deinit()

    # clear - Convenience method used to clear the display
    def clear(self):
        self.lcd.clear()

    # updateScreen - Convenience method used to update the message.
    def updateScreen(self, message):
        self.lcd.clear()
        self.lcd.message = message

    # End class ManagedDisplay definition


# Initialize our display
screen = ManagedDisplay()


# TemperatureMachine - This is our StateMachine implementation class.
# The purpose of this state machine is to manage the three states
# handled by our thermostat:

#  off
#  heat
#  cool
class TemperatureMachine(StateMachine):
    # "A state machine designed to manage our thermostat"

    # Define the three states for our machine.

    #  off - nothing lit up
    #  red - only red LED fading in and out
    #  blue - only blue LED fading in and out
    off = State(initial=True)
    heat = State()
    cool = State()

    # Default temperature setPoint is 72 degrees Fahrenheit
    setPoint = 72

    # cycle - event that provides the state machine behavior
    # of transitioning between the three states of our
    # thermostat
    cycle = (
            off.to(heat) |
            heat.to(cool) |
            cool.to(off)
    )

    # on_enter_heat - Action performed when the state machine transitions
    # into the 'heat' state
    def on_enter_heat(self):
        self.updateLights()

        if DEBUG:
            print("* Changing state to heat")

    # on_exit_heat - Action performed when the statemachine transitions
    # out of the 'heat' state.
    def on_exit_heat(self):
        self.updateLights()

    # on_enter_cool - Action performed when the state machine transitions
    # into the 'cool' state
    def on_enter_cool(self):
        self.updateLights()

        if DEBUG:
            print("* Changing state to cool")

    # on_exit_cool - Action performed when the statemachine transitions
    # out of the 'cool' state.
    def on_exit_cool(self):
        self.updateLights()

    # on_enter_off - Action performed when the state machine transitions
    # into the 'off' state
    def on_enter_off(self):
        redLight.off()
        blueLight.off()

        if DEBUG:
            print("* Changing state to off")

    # processTempStateButton - Utility method used to send events to the
    # state machine. This is triggered by the button_pressed event
    # handler for our first button
    def processTempStateButton(self):
        if DEBUG:
            print("Cycling Temperature State")
        self.cycle()  # move to next state
        self.updateLights()  # refresh LEDs

    # processTempIncButton - Utility method used to update the
    # setPoint for the temperature. This will increase the setPoint
    # by a single degree. This is triggered by the button_pressed event
    # handler for our second button
    def processTempIncButton(self):
        if DEBUG:
            print("Increasing Set Point")
        self.setPoint += 1
        self.updateLights()

    # processTempDecButton - Utility method used to update the
    # setPoint for the temperature. This will decrease the setPoint
    # by a single degree. This is triggered by the button_pressed event
    # handler for our third button
    def processTempDecButton(self):
        if DEBUG:
            print("Decreasing Set Point")
        self.setPoint -= 1
        self.updateLights()

    # updateLights - Utility method to update the LED indicators on the
    # Thermostat
    def updateLights(self):
        # Make sure we are comparing temperatures in the correct scale
        temp = self.temp   # use cached value
        redLight.off()
        blueLight.off()

        # Verify values for debug purposes
        if DEBUG:
            print(f"State: {self.current_state.id}")
            print(f"SetPoint: {self.setPoint}")
            print(f"Temp: {temp}")

        # Determine LED behavior based on state and temperature
        if self.current_state.id == "off":
            # Off state: no indicators
            redLight.off()
            blueLight.off()

        elif self.current_state.id == "heat":
            if temp < self.setPoint:
                # Heating needed: red LED fades in/out
                redLight.pulse()
            else:
                # At or above setpoint: red LED solid on
                redLight.on()

        elif self.current_state.id == "cool":
            if temp > self.setPoint:
                # Cooling needed: blue LED fades in/out
                blueLight.pulse()
            else:
                # At or below set point: blue LED solid on
                blueLight.on()

    # run - kickoff the display management functionality of the thermostat
    def run(self):
        myThread = Thread(target=self.manageMyDisplay)
        myThread.start()

    # Get the temperature in Fahrenheit
    def getFahrenheit(self):
        t = thSensor.temperature
        return ((9 / 5) * t) + 32

    #  Configure output string for the Thermostat Server
    def setupSerialOutput(self):
        # Build comma-delimited string: state, current temp, set point
        output = f"{self.current_state.id},{self.temp},{self.setPoint:.1f}"
        return output

    # Continue display output
    endDisplay = False

    #  This function is designed to manage the LCD
    def manageMyDisplay(self):
        counter = 1
        altCounter = 1

        while not self.endDisplay:
            # Only display if the DEBUG flag is set
            if DEBUG:
                print("Processing Display Info...")

            # Grab the current time
            current_time = datetime.now()

            # Refresh sensor reading once per loop
            self.temp = floor(self.getFahrenheit())

            # Setup display line 1
            lcd_line_1 = current_time.strftime('%b %d  %H:%M:%S\n')

            # Setup Display Line 2
            if altCounter < 6:
                # Show current temperature in °F
                lcd_line_2 = f"Temp: {self.temp}F\n"
                altCounter = altCounter + 1
            else:
                # Show current state and set point in °F
                lcd_line_2 = f"{self.current_state.id} Set:{self.setPoint}F\n"
                altCounter = altCounter + 1
                if altCounter >= 11:
                    # Run the routine to update the lights every 10 seconds
                    # to keep operations smooth
                    self.updateLights()
                    altCounter = 1

            # Update Display
            screen.updateScreen(lcd_line_1 + lcd_line_2)

            # Update server every 30 seconds
            if DEBUG:
                print(f"Counter: {counter}")
            if (counter % 30) == 0:
                # Send current state info to TemperatureServer over UART
                self.setupSerialOutput()

                counter = 1
            else:
                counter = counter + 1
            sleep(1)

        # Cleanup display
        screen.cleanupDisplay()

    # End class TemperatureMachine definition


# Set up our State Machine
tsm = TemperatureMachine()
tsm.run()

# Configure our green button to use GPIO 24 and to execute
# the method to cycle the thermostat when pressed.
greenButton = Button(24)
greenButton.when_pressed = tsm.processTempStateButton

# Configure our Red button to use GPIO 25 and to execute
# the function to increase the set point by a degree.
redButton = Button(25)
redButton.when_pressed = tsm.processTempIncButton

# Configure our Blue button to use GPIO 12 and to execute
# the function to decrease the set point by a degree.
blueButton = Button(12)
blueButton.when_pressed = tsm.processTempDecButton

# Setup loop variable
repeat = True

# Repeat until the user creates a keyboard interrupt (CTRL-C)
while repeat:
    try:
        # wait
        sleep(30)

    except KeyboardInterrupt:
        # Catch the keyboard interrupt (CTRL-C) and exit cleanly
        # we do not need to manually clean up the GPIO pins, the
        # gpiozero library handles that process.
        print("Cleaning up. Exiting...")

        # Stop the loop
        repeat = False

        # Close down the display
        tsm.endDisplay = True
        sleep(1)
```

</details>

- **Download / View Original Code:**  
**[IoT Thermostat](https://github.com/RPG978/RPG978.github.io/tree/main/assets/artifacts)**

---

## Code Review Video

---

## Narrative

---

[<- Back to Portfolio](index.md)
