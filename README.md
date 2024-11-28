# pet-feeder

1) Init Arduino IDE https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html

2) In Arduino IDE Tools -> Partition sheme -> Huge App

3) Telegram -> BotFather -> /newbot -> enter required data (your choice) -> copy token

Telegram -> BotFather -> your new bot -> group privacy -> turn off

Telegram -> Menu -> New Group -> create group with your members, created bot and @getidsbot bot

@getidsbot should send message to the group. Copy group id, remove @getidsbot from the group

After esp32 boot, you have 5 minutes to connect to the created access point (PetFeeder / 11111111), and fill all required data (wifi credentials, bot token and group id).

If everything is correct, you should receive welcome message to your group (after 5 minutes / or after sucess connection). 

4) Optional - modify VoltageSensor::getBatteryPercentage method to match your 18650 voltage graph (not required, but can increase accuracy of battary percentage detection)
