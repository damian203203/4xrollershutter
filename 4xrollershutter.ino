
#define SUPLADEVICE_CPP
#include <SuplaDevice.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>

WiFiClient client;

void connect_to_supla()
{
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);
    char GUID[SUPLA_GUID_SIZE] = {mac[WL_MAC_ADDR_LENGTH - 6], 
        mac[WL_MAC_ADDR_LENGTH - 5], 
        mac[WL_MAC_ADDR_LENGTH - 4], 
        mac[WL_MAC_ADDR_LENGTH - 3], 
        mac[WL_MAC_ADDR_LENGTH - 2], 
        mac[WL_MAC_ADDR_LENGTH - 1]};

    SuplaDevice.begin(GUID,              // Global Unique Identifier 
        mac,               // Ethernet MAC address
        "svr9.supla.org",  // SUPLA server address
        2729,                 // Location ID 
        "7126");               // Location Password
}

void setup() 
{
    Serial.begin(115200);
    delay(10);

    SuplaDevice.addRollerShutterRelays(4, 5);
    SuplaDevice.setRollerShutterButtons(0, 12, 13);

    connect_to_supla();
}

void loop()
{
    SuplaDevice.iterate();
}

// Supla.org ethernet layer
int supla_arduino_tcp_read(void *buf, int count) 
{
    _supla_int_t size = client.available();

    if ( size > 0 ) 
    {
        if ( size > count ) size = count;
        return client.read((uint8_t *)buf, size);
    };

    return -1;
};

int supla_arduino_tcp_write(void *buf, int count) 
{
    return client.write((const uint8_t *)buf, count);
};

bool supla_arduino_svr_connect(const char *server, int port) 
{
    return client.connect(server, 2015);
}

bool supla_arduino_svr_connected(void) 
{
    return client.connected();
}

void supla_arduino_svr_disconnect(void) 
{
    client.stop();
}

void supla_arduino_eth_setup(uint8_t mac[6], IPAddress *ip) 
{
    const char* ssid     = "Sharki";
    const char* password = "mundek08";

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
    }
}

SuplaDeviceCallbacks supla_arduino_get_callbacks(void) 
{
    SuplaDeviceCallbacks cb;

    cb.tcp_read = &supla_arduino_tcp_read;
    cb.tcp_write = &supla_arduino_tcp_write;
    cb.eth_setup = &supla_arduino_eth_setup;
    cb.svr_connected = &supla_arduino_svr_connected;
    cb.svr_connect = &supla_arduino_svr_connect;
    cb.svr_disconnect = &supla_arduino_svr_disconnect;
    cb.get_temperature = NULL;
    cb.get_temperature_and_humidity = NULL;
    cb.get_rgbw_value = NULL;
    cb.set_rgbw_value = NULL;

    return cb;
}
