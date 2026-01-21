**Battery Drain in Zigbee Devices — Experiment Replication**

Overview
--------
This repository contains the materials and instructions to reproduce a battery exhaustion experiment on Zigbee nodes. The experiment demonstrates how an attacker can force a radio module and microcontroller to remain continuously active, drastically reducing the runtime of a battery-powered board.

Status: experimental replication document and test scripts.

Goals
-----
- Demonstrate that targeted frame transmissions at sufficient frequency can induce high power consumption in Zigbee nodes.
- Provide reproducible instructions to validate results and compare traffic profiles (control, stress, saturation).

Materials and hardware
----------------------
- M5Stack boards (or similar with ADC capability).
- XBee Series 2 modules (or equivalent Zigbee modules) for victim nodes and a coordinator.
- USB cable for the coordinator (PC connection).
- Test battery (use the same battery parameters across replicates).

Key files
---------
- [script_msg.py](script_msg.py): Python script used to send API frames to the coordinator and generate traffic profiles.
- m5zigbeebattery/m5zigbeebattery.ino: Arduino/ESP sketch to read battery voltage and log measurements.

Setup and methodology
---------------------
1. Configure the victim XBee modules with the appropriate parameters (e.g., AP=0 for transparent mode) and the coordinator in API mode (AP=1).
2. Connect the coordinator to the PC and verify the available serial port (COMx on Windows).
3. Set the COM port and target MAC addresses, then run the attack script from the PC to generate the traffic profiles:

   - Control: 1 packet every 10 minutes.
   - Stress: 1 packet/second.
   - Saturation: 5 packets/second.

4. Upload the .ino sketch to the M5Stack boards.
5. Monitor and record battery voltage on the victim boards every 5 minutes until failure.

Running the script (PowerShell example)
--------------------------------------
Replace `COM3` with the appropriate port and adjust baud rate if needed.

```powershell
python script_msg.py --port COM3 --baud 115200 --target 0013A20040B2XXXX --rate 1
```

Notes on `script_msg.py`
------------------------
- The script uses `pyserial` to open the serial port and send API frames (Transmit Request 0x10) to the coordinator.
- Ensure the serial port is not occupied by other software (for example, XCTU).

Board sketch (m5zigbeebattery.ino)
---------------------------------
- The sketch reads battery voltage via the ADC and can log or transmit values over serial for later analysis.

Expected results and observations
---------------------------------

<img width="3853" height="2371" alt="chart" src="https://github.com/user-attachments/assets/7914aa3e-e47e-4671-84ab-3cbe81c10ebc" />


<img width="658" height="532" alt="data" src="https://github.com/user-attachments/assets/94a8f894-2984-417d-9fda-d990b4a31526" />

- Experimental data shows a significant reduction in runtime under attack scenarios compared to control.
- An important finding is that above a certain traffic threshold the radio's power consumption stays at its maximum level. Therefore, moderate attack frequencies (e.g., 1 packet/second) can approach the maximum damaging effect without additional network saturation.

---

Created: 2026-01-20



