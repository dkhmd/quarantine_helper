#include "ble_master.h"

BLEDevice peripheral;
BLECharacteristic cmdCharacteristic;

static bool scan_peripheral() {  
  bool ret = false;
  
  BLE.scanForUuid(GATEWAY_SERVICE_UUID);

  // check if a peripheral has been discovered
  peripheral = BLE.available();

  if (!peripheral) {
    goto __fin;
  }

  if (peripheral.hasLocalName() != true) {
    goto __fin;
  }

  if (peripheral.localName() != "Gateway") {
    goto __fin;
  }

  Serial.print("Peripheral Name: ");
  Serial.println(peripheral.localName());

  // print the advertised service UUIDs, if present
  if (peripheral.hasAdvertisedServiceUuid()) {
    Serial.print("Service UUIDs: ");
    for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
      Serial.print(peripheral.advertisedServiceUuid(i));
      Serial.print(" ");
    }
    Serial.println();
  }

  // print the RSSI
  Serial.print("RSSI: ");
  Serial.println(peripheral.rssi());

  ret = true;

__fin:
  BLE.stopScan();
  return ret;
}

static bool connect_peripheral()
{
  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    goto __error; 
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    goto __error;
  }

  // retrieve the command characteristic
  cmdCharacteristic = peripheral.characteristic(SETTING_CHARACTERISTIC_UUID);

  if (!cmdCharacteristic) {
    Serial.println("Peripheral does not have command characteristic!");
    goto __error;
  } else if (!cmdCharacteristic.canWrite()) {
    Serial.println("Peripheral does not have a writable command characteristic!");
    goto __error;
  } else {
    Serial.println("Peripheral is ready");
  }

  return true;

__error:
  peripheral.disconnect();
  return false;
}

void ble_setup() {
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  Serial.print("Local address is: ");
  Serial.println(BLE.address());
  
  Serial.println("BLE central scan");

  // wait for peripheral connection
  while(true) {
    if (scan_peripheral() != true) {
      continue;
    }
    if (connect_peripheral() != true){
      continue;
    }
    break;
  }
}


bool ble_send(byte* buf, int len) {
  bool ret = false;
  
  if (peripheral.connect()) {
    ret = cmdCharacteristic.writeValue(buf, len);
    if (ret != true){
      Serial.println("Failed to tx!");
    }
  } else {
    Serial.println("Failed to connect!");
  }

  if(ret != true){
    Serial.println("Try to reconnect...!");
    connect_peripheral();
  }

  return ret;
}

void ble_get_address(char ble_address[]) {
    char temp[3];
    int i;

    for(i = 0; i < 6; i++){
        temp[0] = BLE.address()[i * 3 + 0];
        temp[1] = BLE.address()[i * 3 + 1];
        temp[2] = 0x00;
        ble_address[i] = strtol(temp, NULL, 16);
    }
}
