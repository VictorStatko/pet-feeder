# Pet Feeder

This project allows you to build an automated pet feeder using an ESP32 microcontroller. The feeder can be controlled via Telegram and is capable of reading battery levels, calibrating a load cell, and sending updates to a Telegram group.

## Setup Instructions

### 1. Install Arduino IDE
Follow the instructions to set up the [Arduino IDE for ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).

### 2. Set Partition Scheme in Arduino IDE
In Arduino IDE, navigate to:
- **Tools** -> **Partition Scheme** -> **Huge App**.

### 3. Create a Telegram Bot
1. Open Telegram and search for **BotFather**.
2. Start a chat with **BotFather** and send the command `/newbot`.
3. Follow the instructions to create a bot (you can choose the bot name and username).
4. Copy the bot token provided by **BotFather**.

### 4. Create a Telegram Group and Get Group ID
1. In Telegram, create a new group.
2. Add your newly created bot and **@getidsbot** to the group.
3. **@getidsbot** will send a message to the group. Copy the group ID from the message and remove **@getidsbot** from the group.

### 5. Connect ESP32 to Wi-Fi
1. After powering on the ESP32, it will broadcast an access point with the name **PetFeeder** and password **11111111**.
2. Connect to this access point within 5 minutes and fill in the required data:
    - Wi-Fi credentials
    - Bot token
    - Group ID

If everything is correct, you will receive a welcome message in your Telegram group, either after a successful connection or after 5 minutes.

### 6. (Optional) Calibrate Battery Percentage
To improve battery percentage accuracy, modify the `VoltageSensor::getBatteryPercentage` method to match your 18650 battery voltage graph. This step is optional but recommended for better accuracy.

### 7. (Optional) Modify Voltage Sensor Multipliers
You can adjust the multipliers for `espVoltageSensor` and `motorVoltageSensor` to match your measurements:
1. Compare the voltage read by `VoltageSensor::readVoltage` to actual measurements using a multimeter.
2. If the multimeter reading is higher than `VoltageSensor::readVoltage`, the multiplier will be greater than 1. Otherwise, it will be less than 1.

### 8. Calibrate Load Cell (Step 1)
1. Use the file `additional/loadCellCalibration1` to calibrate the load cell.
2. Follow the instructions on the Serial Monitor.
3. Calculate the calibration factor using the formula:
    - `calibration factor = (reading) / (known weight)`
4. Store the calibration factor (e.g., 1106).

### 9. Calibrate Load Cell (Step 2)
1. Use the file `additional/loadCellCalibration2` for the second calibration step.
2. This step should be done after embedding the load cell into the 3D printed box (with plate, but without the bowl).
3. Store the offset value (e.g., -186450).

### 10. Setup Load Cell
Use the retrieved calibration values in your code:
```cpp
scale.set_scale(1106);  // Calibration factor
scale.set_offset(-186450);  // Offset value
```