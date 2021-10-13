import argparse
import binascii
import time
from datetime import datetime as dt
from pybleno import *
import publish

ACTION_TOUCH = 1
ACTION_WIPE = 2

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
    else:
        raise Exception('Advertise Error')

def onWriteRequest_set(data, offset, withoutResponse, callback):
    print(len(data), str(data))
    now = dt.now()
    dict_data = {}
    dict_data['date'] = now.strftime('%Y-%m-%d %H:%M:%S')
    dict_data['address'] = prettify(data[0:6])
    # TBD
    dict_data['number_of_beacon'] = 0
    dict_data['beaon'] = []
    dict_data['action'] = convAction(data[63])
    print(dict_data)

    topic = 'device/' + str(binascii.hexlify(data[0:6]), 'utf-8') + '/data'
    print(topic)
    publish.publish(endpoint=args.ep, cert=args.cert, key=args.key, root=args.root, data=dict_data, topic=topic)

def prettify(mac_string):
    return ':'.join('%02x' % b for b in mac_string)

def convAction(str):
    if str == ACTION_TOUCH:
        action = 'touch'
    elif str == ACTION_WIPE:
        action = 'wipe'
    else:
        action = 'none'
    return action

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='python3 gateway.py cert key root data endpoint')

    parser.add_argument('-c', '--cert', required=True, help='cert file path')
    parser.add_argument('-k', '--key', required=True, help='key file path')
    parser.add_argument('-r', '--root', required=True, help='root file path')
    parser.add_argument('-e', '--ep', required=True, help='endpoint')

    args = parser.parse_args()

    bleno = Bleno()
    bleno.on('stateChange', onStateChange)
    bleno.on('advertisingStart', onAdvertisingStart)
    GATEWAYCharacteristic_set = SettingCharacteristic()
    GATEWAYCharacteristic_set.on('writeRequest', onWriteRequest_set)
    bleno.start()
 
    while True:
        time.sleep(1)
