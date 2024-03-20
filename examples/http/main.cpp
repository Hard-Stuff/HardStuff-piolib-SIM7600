
#define SIM7600_APN "id" // for the "IdMobile" access point name in the UK

#include <HardStuff_pio_bricks_SIM7600.hpp>
SIM7600::Client sim_client(0); // create a new unsecure SIM7600 client on MUX 0

#include <ArduinoHttpClient.hpp>
HttpClient http_wiremock(sim_client, "w70l3.wiremockapi.cloud", 80); // Create an HTTP wrapper around the client

void setup()
{
    SIM7600::init();
}

void loop()
{
    http_wiremock.get("/hello_world");
    int status_code = http_wiremock.responseStatusCode();
    http_wiremock.skipResponseHeaders();
    String body = http_wiremock.responseBody();
    http_wiremock.stop();
}