import time
from pybleno import *
 
GATEWAY_SERVICE_UUID = '354d8340-289e-11ec-a32b-531c9f618227'
SETTING_CHARACTERISTIC_UUID = '354d8343-289e-11ec-a32b-531c9f618227'
 
class SettingCharacteristic(Characteristic):
    def __init__(self):
        Characteristic.__init__(self, {
            'uuid': SETTING_CHARACTERISTIC_UUID,
            'properties': ['write'],
            'value': None
        })
 
    def onWriteRequest(self, data, offset, withoutResponse, callback):
        callback(self.RESULT_SUCCESS)
 
 
def onStateChange(state):
    print('on -> stateChange: ' + state)
    if (state == 'poweredOn'):
        bleno.startAdvertising('Gateway', [GATEWAY_SERVICE_UUID])
    else:
        bleno.stopAdvertising()
 
def onAdvertisingStart(error):
    print('on -> advertisingStart1: ' + ('error ' + str(error) if error else 'success'))
    if not error:
        print('onAdvertisingStart')
        bleno.setServices([
            BlenoPrimaryService({
                'uuid': GATEWAY_SERVICE_UUID,
                'characteristics': [
                    GATEWAYCharacteristic_set
                ]
            })
        ]) 
 
def onWriteRequest_set(data, offset, withoutResponse, callback):
    print(str(data))
 
if __name__ == '__main__':
    bleno = Bleno()
    bleno.on('stateChange', onStateChange)
    bleno.on('advertisingStart', onAdvertisingStart)
    GATEWAYCharacteristic_set = SettingCharacteristic()
    GATEWAYCharacteristic_set.on('writeRequest', onWriteRequest_set)
    bleno.start()
    counter = 0
 
    while True:
        time.sleep(1)
