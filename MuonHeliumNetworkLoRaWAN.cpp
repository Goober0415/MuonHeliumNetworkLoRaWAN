// Step 1: Verify your Muon hardware is ready for LoRaWAN
// Based on: https://docs.particle.io/reference/datasheets/m-series/muon-datasheet/
//
// This does two things the datasheet says are required before touching the KG200Z:
//   1. Enables 3V3_AUX (powers the LoRa radio, Ethernet, QWIIC, HAT connector)
//   2. Scans the I2C bus to confirm the KG200Z appears at address 0x61
//
// Expected I2C devices per datasheet:
//   0x28 = STUSB4500 USB-C power controller
//   0x36 = MAX17043 Fuel Gauge
//   0x48 = TMP112A temperature sensor
//   0x61 = KG200Z LoRaWAN radio  <-- this is what we need to see
//   0x69 = AM1805 RTC/Watchdog
//   0x6B = bq24195 PMIC

#include "Particle.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_ALL);

void setup() {
    Serial.begin(9600);
    waitFor(Serial.isConnected, 15000);

    Log.info("=== Muon LoRaWAN Hardware Check ===");

    // Step 1: Enable 3V3_AUX
    // The datasheet says this powers the LoRa radio and defaults to OFF.
    // setup.particle.io does this automatically; we do it manually here.
    // NOTE: This setting is persistent — only needs to be done once per device.
    SystemPowerConfiguration conf;
    conf.auxiliaryPowerControlPin(D13).interruptPin(A6);
    int res = System.setPowerConfiguration(conf);
    Log.info("3V3_AUX power config result: %d (0 = success)", res);

    // Give the KG200Z time to power up
    delay(500);

    // Step 2: Start I2C
    Wire.begin();

    // Step 3: Scan the bus
    Log.info("Scanning I2C bus...");
    int found = 0;
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Log.info("  Found device at 0x%02X %s", addr,
                addr == 0x28 ? "(STUSB4500 USB-C)" :
                addr == 0x36 ? "(MAX17043 Fuel Gauge)" :
                addr == 0x48 ? "(TMP112A Temp Sensor)" :
                addr == 0x61 ? "(KG200Z LoRaWAN radio) <-- GOOD)" :
                addr == 0x69 ? "(AM1805 RTC/Watchdog)" :
                addr == 0x6B ? "(bq24195 PMIC)" : "(unknown)");
            found++;
        }
        delay(5);
    }
    Log.info("Scan complete. Found %d device(s).", found);

    if (found == 0) {
        Log.error("No I2C devices found! 3V3_AUX may not be enabled yet.");
        Log.error("If this is your first run, reset the device and try again.");
    }
}

void loop() {
    // Nothing here yet — get the hardware confirmed first
}