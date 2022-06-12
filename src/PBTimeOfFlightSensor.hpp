#pragma once

#include <Control_Surface.h>
#include <vl53l4cd_class.h>
#include <Wire.h>
#include <WProgram.h>

class PBTimeOfFlightSensor : public MIDIOutputElement
{
public:
    /*
     * @brief   Create a new MyNoteButton object on the given pin, with the
     *          given address and velocity.
     *
     * @param   pin
     *          The digital input pin to read from.
     *          The internal pull-up resistor will be enabled.
     * @param   address
     *          The MIDI address to send to.
     * @param   velocity
     *          The MIDI note velocity [0, 127].
     */
    PBTimeOfFlightSensor(pin_t xshutPin, MIDIChannelCable channel)
        : sensor(&Wire2, xshutPin), channel(channel) {}

public:
    // Initialize: enable the pull-up resistor for the button
    // This method is called once by `Control_Surface.begin()`.
    void begin() final override
    {
        // Serial.begin(9600);

        // Initialize I2C bus.
        Wire2.begin();
        // i2cDev->begin();

        // Serial.println("after i2cdev");

        // Configure VL53L4CD satellite component.
        sensor.begin();

        // Switch off VL53L4CD satellite component.
        sensor.VL53L4CD_Off();

        // Initialize VL53L4CD satellite component.
        sensor.InitSensor();

        // Program the highest possible TimingBudget, without enabling the
        // low power mode. This should give the best accuracy
        sensor.VL53L4CD_SetRangeTiming(10, 0);

        // Start Measurements
        sensor.VL53L4CD_StartRanging();
    }

    // Update: read the button and send MIDI messages when appropriate.
    // This method is called continuously by `Control_Surface.loop()`.
    void update() final override
    {
        VL53L4CD_Result_t results;
        uint8_t status;
        char report[64];
        uint16_t distance;

        status = sensor.VL53L4CD_CheckForDataReady(&newDataReady);

        if ((!status) && (newDataReady != 0))
        {
            // (Mandatory) Clear HW interrupt to restart measurements
            sensor.VL53L4CD_ClearInterrupt();

            // Read measured distance. RangeStatus = 0 means valid data
            sensor.VL53L4CD_GetResult(&results);
            snprintf(report, sizeof(report), "Status = %3u, Distance = %5u mm, Signal = %6u kcps/spad\r\n",
                     results.range_status,
                     results.distance_mm,
                     results.signal_per_spad_kcps);
            // Serial.print(report);

            rangeStatus = results.range_status;

            newDataReady = 0;

            if (results.range_status == 0) {
                distance = results.distance_mm;

                distance = constrain(distance, MIN_DISTANCE, MAX_DISTANCE);

                pitchBendValue = map(distance, MIN_DISTANCE, MAX_DISTANCE, MIN_PITCH_BEND, MAX_PITCH_BEND);

                pitchBendValue = filter.filter(pitchBendValue);
                // Serial.println(pitchBendValue);
                Control_Surface.sendPitchBend(channel, pitchBendValue);

            }
            else if (prevRangeStatus == 0 || rangeStatus == 1 || rangeStatus == 2) 
            {
                // Don't change the pitch bend value if we just changed out of
                // the valid data state. The sensor will sometimes get a sample
                // or two that aren't valid, and then go back to valid. We don't
                // want the pitch bend to rapidly change when this happens.

                // range status == 0 -> okay
                // range status == 1 -> warning: signal is above defined threshold
                // range status == 2 -> warning: signal is below defined threshold
                //https://www.st.com/resource/en/user_manual/um2931-a-guide-to-using-the-vl53l4cd-ultra-lite-driver-uld-stmicroelectronics.pdf


                // Serial.println("prevRangeStatus == 0");
                // Serial.println(pitchBendValue);
                // initialize the filter with the pitch bend value
                // pitchBendValue = filter.filter(pitchBendValue);
                // Serial.println(pitchBendValue);
                // Control_Surface.sendPitchBend(channel, pitchBendValue);
            }
            else
            {
                // filter back to zero pitch bend
                pitchBendValue = filter.filter(8192) ;
                // Serial.println("prevRangeStatus != 0");
                // Serial.println(pitchBendValue);
                Control_Surface.sendPitchBend(channel, pitchBendValue);
            }

            prevRangeStatus = rangeStatus;

        }
        else {
            // Data isn't ready yet
            // Serial.println("data not ready!");
        }
        // Serial.println("test");


        }
    

private:
    TwoWire *i2cDev;
    VL53L4CD sensor;
    uint16_t pitchBendValue = 8192;
    uint8_t rangeStatus;
    uint8_t prevRangeStatus;
    EMA<2, uint16_t, uint32_t> filter;
    const MIDIChannelCable channel;
    uint8_t newDataReady = 0;
    const uint16_t MIN_DISTANCE = 20;
    const uint16_t MAX_DISTANCE = 700;
    const uint16_t MIN_PITCH_BEND = 0;
    const uint16_t MAX_PITCH_BEND = 16383;
};