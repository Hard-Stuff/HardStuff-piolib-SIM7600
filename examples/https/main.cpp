#include <configs/wiremock.h> // Define your wiremock details in a config

#include <HardStuff_pio_bricks_SIM7600.hpp>
SIM7600::ClientSecure sim_client(0); // create a new secure SIM7600 client (SSL-secure ready), on MUX 0.

#include <HardStuff_pio_lib_Http.hpp>
HardStuffHttpClient http_wiremock(sim_client, WIREMOCK_SERVER, WIREMOCK_PORT); // Create an HTTP wrapper around the client

void setup()
{
    SIM7600::init();
    sim_client.setCACert(WIREMOCK_CERT_CA); // Set the SSL certificate for secure communication between client and server.
}

void loop()
{
    HttpRequest request;
    request.addHeader("Connection", "close");
    request.addParam("device_mac", getMacAddress());
    request.body = "Hi there, how are you?";
    http_wiremock.postToHTTPServer("/friendly_wave").print(); // Perform a get request on the /hello_world endpoint and print the HTTP response.
    delay(1000);
}