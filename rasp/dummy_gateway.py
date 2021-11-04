import argparse
from time import gmtime, strftime, sleep
from datetime import datetime as dt
import publish
import random
import ibscanner

devaddr = "01:23:45:67:89:AB"
ibinfo = {}

def prettify(mac_string):
    return ':'.join('%02x' % b for b in mac_string)

def getRaondomAction():
    l = ['none', 'touch', 'wipe']
    return random.choice(l)

def pub():
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

    dict_data['action'] = getRaondomAction()

    topic = 'device/' + devaddr.replace(':', '') + '/data'
    publish.publish(endpoint=args.ep, cert=args.cert, key=args.key, root=args.root, dict_data=dict_data, topic=topic)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='sudo python3 dummy_gw.py interval cert key root data endpoint')

    parser.add_argument('-i', '--interval', default=5, help='interval(sec)')
    parser.add_argument('-c', '--cert', required=True, help='cert file path')
    parser.add_argument('-k', '--key', required=True, help='key file path')
    parser.add_argument('-r', '--root', required=True, help='root file path')
    parser.add_argument('-e', '--ep', required=True, help='endpoint')
    parser.add_argument('-u', '--uuid', default='00000000-e132-1001-b000-001c4de2af03', help='uuid')

    args = parser.parse_args()

    while True:
        ib = ibscanner.iBeacon(uuid=args.uuid)
        ibinfo = ib.exec_scan()
        pub()
        sleep(args.interval)
