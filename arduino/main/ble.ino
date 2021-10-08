#include <ArduinoBLE.h>

BLEService emgService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE EMG Service

// BLE EMG Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

BLEDevice central;

void setup_ble() {
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("ArduinoBLE");
  BLE.setAdvertisedService(emgService);

  // add the characteristic to the service
  emgService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(emgService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE EMG Peripheral");

  // if a central is connected to peripheral:
  while(1) {
    // listen for BLE peripherals to connect:
    central = BLE.central();
    
    if (central) {
      Serial.print("Connected to central: ");
      // print the central's MAC address:
      Serial.println(central.address());
      break;
    } else {
      yield();
    }
  }
}

void loop_ble() {
  // while the central is still connected to peripheral:
  if (central.connected()) {
    if (switchCharacteristic.written()) {
      if (switchCharacteristic.value()) {   // any value other than 0
        Serial.println("Input value except 0");
      } else {                              // a 0 value
        Serial.println("Input 0");
      }
    }
  } else {
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
