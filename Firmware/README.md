## Arduino Environment Setup

1. In the Arduino IDE, open `File>Preferences` and add `https://mcudude.github.io/MegaCore/package_MCUdude_MegaCore_index.json` to the Additional Boards Manager URLs
2. Open `Tools>Board>Boards Manager...`, search MegaCore, and install it
3. Copy contents of the `Environment` directory (in this repo) to your sketchbook location (usually `Documents\Arduino`)
4. Restart the Arduino IDE (or enter and leave the boards manager)
5. Select 'RRS MMD Tracker' as the target board
6. Configure desired compiler optimization level and configure link-time optimizations (LTO)

## Flashing the Bootloader on New Boards

1. Program another arduino with the ArduinoISP sketch in `Examples>11. ArduinoISP>ArduinoISP`
2. Connect that arduino to the ICSP header on the tracker as for normal ICSP
3. Select the 'RRS MMD Tracker' device in the Tools menu.  Leave the COM port set to the normal arduino
4. Choose 'Arduino as ISP' from the Tools>Programmer selector
5. Hit 'Burn Bootloader'