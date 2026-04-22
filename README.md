Particle Muon + LoRaWAN + Helium
Network Starter Project
OTAA join • Periodic uplinks • Downlink handling • Helium Console decoder
Overview
This starter project lets a Particle Muon (M-SoM) join the Helium LoRaWAN network using OTAA, send a compact 8-byte sensor payload every 60 seconds, and handle downlink commands. The project is self-contained and does not require the Particle Cloud.

Project Structure
muon-helium-lorawan/
├── src/
│   ├── main.cpp          ← Firmware (OTAA join + periodic uplink)
│   └── credentials.h     ← AppEUI / DevEUI / AppKey  [gitignored]
├── helium/
│   └── decoder.js        ← Paste into Helium Console → Functions
├── project.properties        ← Particle library dependencies
├── .gitignore
└── README.md

Prerequisites
Tool / Account	How to get it
Particle CLI	npm install -g particle-cli
Particle account	particle.io → Sign Up
Helium Console account	console.helium.com
LoRa radio shield/wing	Compatible with Muon SOM pinout

Step 1 — Register your device on Helium Console
1.	Log into Helium Console (console.helium.com).
2.	Devices → + Add Device → choose OTAA.
3.	Copy the three generated keys: AppEUI, DevEUI, AppKey.
4.	Open src/credentials.h and replace the placeholder zero arrays with your keys (MSB byte order, which Console shows by default).

⚠️  Security Warning
credentials.h is listed in .gitignore. Never commit real keys to version control. Rotate keys immediately if accidentally exposed.

Step 2 — Add the decoder in Helium Console
5.	Helium Console → Functions → + Add Function.
6.	Type: Decoder, Format: Custom Script.
7.	Paste the contents of helium/decoder.js.
8.	Attach the function to your device or a Label.

Step 3 — Install Particle CLI & log in
npm install -g particle-cli
particle login

Step 4 — Add the LoRaWAN library
cd muon-helium-lorawan
particle library add LoRaWAN
If the library name differs in the registry, search with:
particle library search lora

Step 5 — Compile & Flash
Over USB (recommended for first flash)
Put the Muon into DFU mode: hold MODE, tap RESET, release MODE when the LED blinks yellow.

# Compile for Muon
particle compile muon . --saveTo firmware.bin

# Flash over USB/serial
particle flash --serial firmware.bin

Over-the-Air (optional — after device is claimed)
particle flash <your-device-name> .

Step 6 — Monitor Serial Output
particle serial monitor --follow

Expected output after a successful join:
[BOOT] Particle Muon — LoRaWAN / Helium starter
[LORA] Attempting OTAA join...
[LORA] ✓ Joined Helium network (OTAA)
[TX]   8 bytes queued on port 1
[TX]   Transmission complete (ack=no/unconfirmed)

Payload Format
The 8-byte uplink frame is encoded as follows:

Bytes	Field	Type	Scale	Unit
0–1	temperature	int16 big-endian	÷ 100	°C
2–3	humidity	uint16 big-endian	÷ 100	%RH
4–7	uptime	uint32 big-endian	raw	seconds

Adding Real Sensors
Replace the placeholder values in buildPayload() inside main.cpp. Example with a BME280:

// Before (placeholder)
float temperature = 22.5f;
float humidity    = 55.0f;

// After — BME280 via I²C
#include "Adafruit_BME280.h"
Adafruit_BME280 bme;
// In setup(): bme.begin(0x76);
float temperature = bme.readTemperature();
float humidity    = bme.readHumidity();

Tuning LoRaWAN Parameters
Parameter	Where	Notes
Region	LoRaWAN.begin(...)	US915, EU868, AU915, AS923 …
Sub-band	LoRaWAN.setSubBand(2)	Helium US915 uses sub-band 2
ADR	LoRaWAN.setADR(true)	Let Helium pick optimal data rate
Confirmed uplinks	LoRaWAN.send(..., true)	Use sparingly — costs airtime
Send interval	SEND_INTERVAL_MS	Min ~30 s; respect fair-use limits

Helium Integrations
Once data appears in Console, forward it to downstream services via Console → Integrations:

•	HTTP Integration — any REST endpoint or webhook
•	AWS IoT Core
•	Google Cloud IoT
•	Azure IoT Hub
•	MQTT — any broker

Troubleshooting
Symptom	Fix
Join fails repeatedly	Check sub-band (must be 2 for US915). Verify keys in credentials.h are MSB order.
No data in Helium Console	Ensure your gateway is online. Check device coverage map at explorer.helium.com.
Decoder shows raw bytes	Confirm decoder function is attached to the correct device or Label.
Compile error: LoRaWAN.h not found	Run: particle library add LoRaWAN

License
MIT — use freely, improve, and share.

