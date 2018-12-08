
#define SUPLADEVICE_CPP
#include <SuplaDevice.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ShiftRegister74HC595.h>

WiFiClient client;
ShiftRegister74HC595 shift_register(1, 15, 16, 0); 

struct ShiftRegisterWrapper
{
    int mem[8];
    ShiftRegisterWrapper()
    {
        for (int i=0; i < 8; i++)
            mem[i] = 0;
    }
    void set(uint8_t pin, uint8_t val)
    {
        Serial.print("SET, pin: ");
        Serial.print(pin);
        Serial.print(", val: ");
        Serial.print(val);
        Serial.print("\n");
        shift_register.set(pin, val);
        mem[pin] = val;
    }
    int get(uint8_t pin)
    {
        int val = mem[pin];
        Serial.print("GET, pin: ");
        Serial.print(pin);
        Serial.print(", val: ");
        Serial.print(0);
        Serial.print("\n");
        return val;
    }
} shift_register_with_memory;

void customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val)
{
    if (pin > 100)
    {
        shift_register_with_memory.set(pin - 101, val);
    }
    else
    {
        digitalWrite(pin, val);
    }
}

int customDigitalRead(int channelNumber, uint8_t pin)
{
    if (pin > 100)
    {
        return shift_register_with_memory.get(pin - 101);
    }
    else
    {
        return digitalRead(pin);
    }
}

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

    SuplaDevice.setDigitalWriteFuncImpl(&customDigitalWrite);
    SuplaDevice.setDigitalReadFuncImpl(&customDigitalRead);

    SuplaDevice.addRollerShutterRelays(14, 2);
    SuplaDevice.setRollerShutterButtons(0, 13, 12);

    SuplaDevice.addRollerShutterRelays(101, 102);
    SuplaDevice.setRollerShutterButtons(1, 4, 5);

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
