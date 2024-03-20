# **Hard Stuff** bricks: SIMCOM SIM7600 TinyGSM wrapper

This brick _(a non-classable wrapper around a `lib`)_ is designed to abstract complexity away from using the SIM7600 module with TinyGSM, specifically for the 4G and GPS functionalities. This means:

-   calling high-level functions (e.g. `updateTime` of the ESP32 from NTP) rather than low-level functions (e.g.`modem.getGPSTime` then unpacking the time individually then `setTime` then etc...)
-   standardise `#define` definitions, to make code more legible and repeatable.

## Usage

Hard Stuff bricks are written as namespaces. This is because any given project will likely only have one brick (e.g. it's unlikely your design will incorporate two SIM7600 modules), and in this case also wraps the Secure and Unsecure variants of the TinyGSM Client class.

### Basic Example

This example uses this `SIM7600` brick and our `Http` lib [available here](https://github.com/Hard-Stuff/HardStuff_pio_lib_Http) to perform secured GET requests in as few lines of code as possible.

All functionality and classes are available within the `SIM7600` namespace.

```cpp
#include <configs/wiremock.h> // Define your wiremock details in a config

#include <HardStuff_pio_bricks_SIM7600.hpp>
SIM7600::ClientSecure sim_client(0); // create a new secure SIM7600 client (SSL-secure ready), on MUX 0

#include <HardStuff_pio_lib_Http.hpp>
HardStuffHttpClient http_wiremock(sim_client, WIREMOCK_SERVER, WIREMOCK_PORT); // Create an HTTP wrapper around the client

void setup() {
    SIM7600::init();
    sim_client.setCACert(WIREMOCE_CERT_CA); // Set the SSL certificate for secure communication
}

void loop() {
    http_wiremock.getFromHTTPServer("/hello_world").print(); // Perform a get request on the /hello_world endpoint and print the HTTP response.
    delay(1000);
}
```

### `#define`s to be aware of

You can modify the behaviour of the client and SIM7600 via the standardised defines.

```cpp
// Required defines
#define SIM7600_APN "your_apn" // The Access Point Name (e.g. Verizon / Three) of your SIM card

// Optional defines
#define SIM7600_BAUD number    // Baud rate of your SIM7600 devices, defaults to 115200
#define SIM7600_SERIAL number  // Serial channel you want to use, default is Serial1 (with Serial being reserved for your Serial.print()s)
#define SIM7600_UART_RX number // RX pin you're using between your microcontroller and the SIM7600, default varies depending on microcontroller.
#define SIM7600_UART_TX number // TX pin you're using between your microcontroller and the SIM7600, default varies depending on microcontroller.

// Inherited from TinyGSM
#define DUMP_AT_COMMANDS          // Will dump your AT commands to Serial. Very useful for debugging, remove in production code!
#define TINY_GSM_RX_BUFFER number // Modify the RX buffer size (default is 1024). Depends on your ,microcontroller.
```

### Client classes available

-   `SIM7600::ClientSecure` is for establishing secure (SSL) connections. You will likely need to set CA and perhaps user Certs prior to establishing the connection (e.g. `SecureClient.setCACert(AWS_ROOT_CA)`). You typically use this for http connections to port 443, and MQTT connections to port 8883.
-   `SIM7600::Client` is for establishing unsecure connections, or for deploying your own SSL wrapper around the client. You typically use this for http connections to port 80, and MQTT connections to port 1883.

### Functions available

-   `init()` initialises the SIM7600 module, including activating the Serial communication, waiting-for and connecting-to the network, connecting to GPRS, setting full phone functionality, and enabling the GPS.
-   `updateTime()` uses the GPS to update the system time. If this is not possible, we use the network time. (You can also leverage NTPServerSync which is not accounted for yet).
-   `getGPSCoordinates()` returns a `GPSResponse` struct, which contains the latitude, longitude, speed, date, and accuracy. This struct itself can return if it's `withinAccuracy` or `print` to Serial.


### Dependencies

- paulstoffregen/Time
- vshymanskyy/StreamDebugger
- TinyGSM **[the Hard Stuff PR](github.com:Hard-Stuff/TinyGSM.git)**! (As this includes native SSL support for the SIM7600).


## Hard Stuff

Hard Stuff is a hardware prototyping agency and venture studio focussing on sustainability tech, based in London, UK.
Find out more at [hard-stuff.com](hard-stuff.com).