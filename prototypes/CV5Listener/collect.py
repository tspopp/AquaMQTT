import os
import time
import datetime
import paho.mqtt.client as mqtt

last_payloads = {}

def on_message(client, userdata, message):
    topic = message.topic
    payload = message.payload

    hex_file_name = topic.replace('/', '_') + '_hex.csv'
    dec_file_name = topic.replace('/', '_') + '_dec.csv'

    hex_payload = payload.decode('utf-8')

    if topic in last_payloads and last_payloads[topic] == hex_payload:
        return

    timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')

    with open(hex_file_name, 'a') as hex_file:
        hex_file.write(timestamp + ',' + hex_payload + '\n')

    dec_payload = ' '.join([str(int(payload[i:i+2], 16)) for i in range(0, len(payload), 2)])

    with open(dec_file_name, 'a') as dec_file:
        dec_file.write(timestamp + ',' + dec_payload + '\n')

    last_payloads[topic] = hex_payload

client = mqtt.Client()

client.username_pw_set(username='your-broker-user', password='your-broker-pass')
client.connect('your-broker-addr', 1883)
client.subscribe('your-prefix/cv5/+/+/+/+/debug')

client.on_message = on_message

client.loop_start()

while True:
    time.sleep(1)

