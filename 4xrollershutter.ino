
#define SUPLADEVICE_CPP
#include <SuplaDevice.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ShiftRegister74HC595.h>

WiFiClient client;

struct ShiftRegisterWithMemoryState
{
    static const uint8_t NR_OF_REGISTERS = 1;//ustaw
    static const uint8_t DATA_PIN = 15;//ustaw
    static const uint8_t CLOCK_PIN = 16;//ustaw
    static const uint8_t LATCH_PIN = 1;//ustaw

    static const uint8_t MEMORY_STATE_SIZE = NR_OF_REGISTERS * 8;
    int memory_state[MEMORY_STATE_SIZE];
    
    ShiftRegister74HC595 shift_register = ShiftRegister74HC595(
        NR_OF_REGISTERS, DATA_PIN, CLOCK_PIN, LATCH_PIN);

    ShiftRegisterWithMemoryState()
    {
        for (int i=0; i < MEMORY_STATE_SIZE; i++)
            memory_state[i] = 0;
    }
    void set(uint8_t pin, uint8_t val)
    {
        shift_register.set(pin, val);
        memory_state[pin] = val;
    }
    int get(uint8_t pin)
    {
<<<<<<< HEAD
        return memory_state[pin];
=======
        return mem[pin];
>>>>>>> parent of 974e3f6... Working rollershutter
    }
} shift_register_with_memory;

void customDigitalWrite(int channelNumber, uint8_t pin, uint8_t val)
{
    if (pin > 100)
    {
        int shift_register_pin = pin - 101;

        Serial.print("Shift regiter write: channelNumber: ");
        Serial.print(channelNumber);
        Serial.print(", PIN: ");
        Serial.print(pin);
        Serial.print(", value: ");
        Serial.print(val);

        Serial.print(", Shift register PIN: ");
        Serial.print(shift_register_pin);

        shift_register_with_memory.set(shift_register_pin, val);
    }
    else
    {
        Serial.print("Standard digital write: channelNumber: ");
        Serial.print(channelNumber);
        Serial.print(", PIN: ");
        Serial.print(pin);
        Serial.print(", value: ");
        Serial.print(val);
        digitalWrite(pin, val);
    }
}

int customDigitalRead(int channelNumber, uint8_t pin)
{
    Serial.print("Digital read called, channelNumber: ");
    Serial.print(channelNumber);
    Serial.print(", PIN: ");
    Serial.print(pin);
    Serial.print("\n");
    if (pin > 100)
    {
        Serial.print("Digital read from Shift register");
        return shift_register_with_memory.get(pin);
    }
    else
    {
        int ret = digitalRead(pin);
        Serial.print("return: ");
        Serial.print(ret);
        return ret;
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

    SuplaDevice.setDigitalWriteFuncImpl(&customDigitalWrite);
    SuplaDevice.setDigitalReadFuncImpl(&customDigitalRead);

    SuplaDevice.setName("4xRS");

    SuplaDevice.begin(GUID,              // Global Unique Identifier 
        mac,               // Ethernet MAC address
        "svr9.supla.org",  // SUPLA server address
        2729,                 // Location ID 
        "7126");               // Location Password
}

void add_devices_to_supla()
{
    SuplaDevice.addRollerShutterRelays(
        101, // Relay 1 (if shift register add 101) 
        102);
    SuplaDevice.setRollerShutterButtons(
        0, // channel
        5, // button 1
        4); // button 2`

   SuplaDevice.addRollerShutterRelays(
        103, // Relay 1 (if shift register add 101) 
        104);
    SuplaDevice.setRollerShutterButtons(
        1, // channel
        2, // button 1
        14); // button 2`

    SuplaDevice.addRollerShutterRelays(
        105, // Relay 1 (if shift register add 101) 
        106);
    SuplaDevice.setRollerShutterButtons(
        2, // channel
        12, // button 1
        13); // button 2`

   SuplaDevice.addRollerShutterRelays(
        107, // Relay 1 (if shift register add 101) 
        108);
    SuplaDevice.setRollerShutterButtons(
        3, // channel
        3, // button 1
        1); // button 2`

}

void setup() 
{
    Serial.begin(115200);
    delay(10);

<<<<<<< HEAD
    add_devices_to_supla();
=======
    SuplaDevice.setDigitalWriteFuncImpl(&customDigitalWrite);
    SuplaDevice.setDigitalReadFuncImpl(&customDigitalRead);

    SuplaDevice.addRollerShutterRelays(14, 2);
    SuplaDevice.setRollerShutterButtons(0, 13, 12);

//    SuplaDevice.addRollerShutterRelays(101, 102);
//    SuplaDevice.setRollerShutterButtons(1, 4, 5);


>>>>>>> parent of 974e3f6... Working rollershutter
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
