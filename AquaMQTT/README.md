# AquaMQTT

AquaMQTT is designed for an ESP32 microcontroller running Arduino. The project offers two distinct operation modes: "
Listener" and "Man-in-the-Middle."

### Operation Modes

1. **Listener Mode:**
    - AquaMQTT acts as an observer, monitoring communication between a heat pump HMI controller and the heat pump main
      controller.
    - Parses the serial messages exchanged between the two controllers and publishes relevant data to specified MQTT
      topics (refer to [MQTT.md](../MQTT.md)).

2. **Man-in-the-Middle Mode:**
    - AquaMQTT intercepts and sits between the communication channels of the HMI controller and the main heat pump
      controller. 
    - Parses the serial messages exchanged between the two controllers and publishes relevant data to specified MQTT
      topics (refer to [MQTT.md](../MQTT.md)).
    - Enables the modification of communication messages in both directions, allowing for the alteration of
      various parameters such as operation mode, water target temperature, and more.

## Getting Started

### Hardware Requirements

- [Arduino ESP32 Nano](https://docs.arduino.cc/hardware/nano-esp32)
- [AquaMQTT Board](../pcb/)

### Installation

1. Clone the AquaMQTT repository to your local machine.

   ```bash
   git clone https://github.com/tspopp/AquaMQTT.git
   ```

2. Configure WiFi and MQTT Settings:

   Open the `include/config/ExampleConfiguration.h` [file](include/config/ExampleConfiguration.h) and update the following parameters to match your WiFi and MQTT broker
   settings:

   ```c++
   namespace aquamqtt
   {
   namespace config
   {
      constexpr char     ssid[]           = "YourWiFiSSID";
      constexpr char     psk[]            = "YourWiFiPassword";
      constexpr char     brokerAddr[]     = "192.168.188.1";
      constexpr uint16_t brokerPort       = 1883;
      constexpr char     brokerClientId[] = "aquamqtt";
      // Leave blank if your broker does not require a username/password
      constexpr char brokerUser[]     = "";
      constexpr char brokerPassword[] = "";
      constexpr char mqttPrefix[]     = "";
   }  // namespace config
   }  // namespace aquamqtt
   ```

   Ensure that the WiFi SSID, password, MQTT broker address, and other parameters are correctly set.

   Additionally, set the operation mode within `include/config/Configuration.h` to either LISTENER or MITM, depending of your [Wiring Configuration](../WIRING.md).

   ```c++
   constexpr EOperationMode OPERATION_MODE = EOperationMode::MITM;
   ```

### Flashing

1. Install PlatformIO IDE

   https://platformio.org/install

2. Build the project using

   ```bash
   pio run -e arduino_nano_esp32
   ```

3. Connect the Arduino and upload the project to your microcontroller via USB

   ```bash
   pio run -t upload -e arduino_nano_esp32
   ```

**Note:** If you are using the cheaper esp32 [esp32-s3-devkit-c1](https://github.com/tspopp/AquaMQTT/issues/56) instead of the Arduino Nano ESP32, use the environment `esp32-s3-devkitc-1` instead of `arduino_nano_esp32`

**Hint:** If you have issues flashing Arduino Nano ESP32, just press the RST button multiple times quickly, until it starts blinking green. It shouldn't be needed, but if the esp is in some strange condition, it can be helpful. See Arduino Bootloader Mode in the official docs: https://docs.arduino.cc/tutorials/nano-esp32/cheat-sheet/

### Over-The-Air Update

1. Determine the IP-Address of your AquaMQTT device

   **Note:** If you already have MQTT up and running, AquaMQTT will publish the IP-Address to `aquamqtt/stats/ipAddress`

2. Adapt `platformio.ini` accordingly:
   ```ini
   # uncomment the below lines to use over the air update
   upload_protocol = espota
   upload_port = 192.168.188.62
   ```

3. Upload
    ```bash
   pio run -t upload -e arduino_nano_esp32
      ```


## Contributions
Contributions to the AquaMQTT project are welcome. Feel free to open issues, submit pull requests, or provide feedback.


## License
This project is licensed under the Apache License 2.0.