import argparse
from awscrt import io, mqtt, auth, http
from awsiot import mqtt_connection_builder
import time as t
import json


def publish(endpoint, cert, key, root, data, topic):
    # Define ENDPOINT, CLIENT_ID, PATH_TO_CERT, PATH_TO_KEY, PATH_TO_ROOT, MESSAGE, TOPIC, and RANGE
    ENDPOINT = endpoint
    CLIENT_ID = "Gateway"
    PATH_TO_CERT = cert
    PATH_TO_KEY = key
    PATH_TO_ROOT = root
    TOPIC = topic

    # Spin up resources
    event_loop_group = io.EventLoopGroup(1)
    host_resolver = io.DefaultHostResolver(event_loop_group)
    client_bootstrap = io.ClientBootstrap(event_loop_group, host_resolver)
    mqtt_connection = mqtt_connection_builder.mtls_from_path(
                endpoint=ENDPOINT,
                cert_filepath=PATH_TO_CERT,
                pri_key_filepath=PATH_TO_KEY,
                client_bootstrap=client_bootstrap,
                ca_filepath=PATH_TO_ROOT,
                client_id=CLIENT_ID,
                clean_session=False,
                keep_alive_secs=6
                )
    print("Connecting to {} with client ID '{}'...".format(
            ENDPOINT, CLIENT_ID))
    # Make the connect() call
    connect_future = mqtt_connection.connect()
    # Future.result() waits until a result is available
    connect_future.result()
    print("Connected!")
    # Publish message to server desired number of times.
    print('Begin Publish')
    mqtt_connection.publish(topic=TOPIC, payload=json.dumps(data), qos=mqtt.QoS.AT_LEAST_ONCE)
    print("Published: '" + json.dumps(data) + "' to the topic: " + topic)
    disconnect_future = mqtt_connection.disconnect()
    disconnect_future.result()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='python3 publish.py cert key root data topic endpoint')

    parser.add_argument('-c', '--cert', required=True, help='cert file path')
    parser.add_argument('-k', '--key', required=True, help='key file path')
    parser.add_argument('-r', '--root', required=True, help='root file path')
    parser.add_argument('-d', '--data', required=True, help='data file path')
    parser.add_argument('-t', '--topic', required=True, help='topic name')
    parser.add_argument('-e', '--ep', required=True, help='endpoint')

    args = parser.parse_args()

    # open data file(json)
    with open(args.data) as f:
        df = json.load(f)

    publish(endpoint=args.ep, cert=args.cert, key=args.key, root=args.root, data=df, topic=args.topic)