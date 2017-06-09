# Building Automation Project
for Mouser Electronics
by Mike Parks

This repo holds all the information needed to experiment with an IoT-based building automation system that leverages the TE Connectivty Weather Shield.

Current Architecture:
- Arduino 101, 8266 WiFi Shield, TE Connectivity Weather Shield: Sensor package monitors temperature, pressure, and relative humidity. Transmits data to a cloud-based backend.
- Huzzah 8266 monitors data from cloud and makers decision to control ventilation dampers based on presets and current sensor reading.
- Leverage the Ubidots cloud service for the backend.  https://ubidots.com

Bill of Materials (BOM) in a pre-built shopping cart for your convenience:
http://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=098ee3c674
