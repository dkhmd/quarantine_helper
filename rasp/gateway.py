import argparse
import sys
from time import gmtime, strftime, sleep
from datetime import datetime as dt
from bluepy import btle
from bluepy.btle import Scanner, DefaultDelegate, BTLEException
import publish
import ibscanner

ACTION_TOUCH = b'\x01'
ACTION_WIPE = b'\x02'

GESTURE_SERVICE_UUID = "c14debb0-38a7-11ec-ba9c-93b3f272a4e7"
GESTURE_CHARACTERISTIC_UUID = "c14debb1-38a7-11ec-ba9c-93b3f272a4e7"

devaddr = None
ibinfo = {}

def prettify(mac_string):
    return ':'.join('%02x' % b for b in mac_string)

def convAction(data):
    if data == ACTION_TOUCH:
        action = 'touch'
    elif data == ACTION_WIPE:
        action = 'wipe'
    else:
        action = 'none'
    return action

class NotifyDelegate(DefaultDelegate):
    def __init__(self, params):
        btle.DefaultDelegate.__init__(self)
            
    def handleNotification(self, cHandle, data):
        now = dt.now()
        dict_data = {}
        dict_data['date'] = now.strftime('%Y-%m-%d %H:%M:%S')
        dict_data['address'] = devaddr

        dict_data['number_of_beacon'] = len(ibinfo)
        dict_data['beaon'] = []
        for k, v in ibinfo.items():
            binfo = {}
            binfo['address'] = k
            binfo['rssi'] = v[0]
            binfo['major'] = v[1]['major']
            binfo['minor'] = v[1]['minor']
            binfo['uuid'] = v[1]['uuid']
            dict_data['beaon'].append(binfo)

        dict_data['action'] = convAction(data)

        topic = 'device/' + devaddr.replace(':', '') + '/data'
        publish.publish(endpoint=args.ep, cert=args.cert, key=args.key, root=args.root, dict_data=dict_data, topic=topic)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='sudo python3 gateway.py cert key root data endpoint')

    parser.add_argument('-c', '--cert', required=True, help='cert file path')
    parser.add_argument('-k', '--key', required=True, help='key file path')
    parser.add_argument('-r', '--root', required=True, help='root file path')
    parser.add_argument('-e', '--ep', required=True, help='endpoint')
    parser.add_argument('-u', '--uuid', default='00000000-e132-1001-b000-001c4de2af03', help='uuid')

    args = parser.parse_args()

    ib = ibscanner.iBeacon(uuid=args.uuid)
    # ib.start_scan()

    while True:
        try:
            scanner = btle.Scanner(0)
            devices = scanner.scan(1.0, passive=True)
            for device in devices:
                for (adtype, desc, value) in device.getScanData():
                    if value != GESTURE_SERVICE_UUID:
                        continue
                    print(' (%3s) %s : %s ' % (adtype, desc, value))
                    # try to connect
                    peri = btle.Peripheral()
                    peri.connect(device.addr, btle.ADDR_TYPE_PUBLIC)
                    devaddr = device.addr
                    print('connected: ' + device.addr)
            if devaddr is None:
                continue

            # check handle
            peri.withDelegate(NotifyDelegate(btle.DefaultDelegate))                    
            charas = peri.getCharacteristics()
            for chara in charas:
                if chara.uuid != GESTURE_CHARACTERISTIC_UUID:
                    continue
                peri.writeCharacteristic(chara.getHandle() + 1, b'\x01\x00')

                while True:
                    # wait for notifications
                    ibinfo = ib.exec_scan()
                    peri.waitForNotifications(1.0)

        except BTLEException:
            print('BTLE Exception while scannning.')
