import sys
import argparse
import serial
import struct
from collections import namedtuple


CONFIG_TYPE = 0x01
FORCE_CONFIG_TYPE = 0x02
VELOCITY_TYPE = 0x03
FORCE_VELOCITY_TYPE = 0x04
ERROR_TYPE = 0x05
INFO_TYPE = 0x06
FORCE_INFO_TYPE = 0x07

MAGIC = 0xAA

CONFIG_REQUEST_LENGTH = 5
CONFIG_RESPONSE_LENGTH = 5
VELOCITY_REQUEST_LENGTH = 5
VELOCITY_RESPONSE_LENGTH = 5
INFO_REQUEST_LENGTH = 4
INFO_RESPONSE_LENGTH = 5
ERROR_LENGTH = 5


InfoResponse = namedtuple('InfoResponse', 'magic, type, address, velocity, crc')
InfoResponseRegexp = 'B'*INFO_RESPONSE_LENGTH
InfoRequest = namedtuple('InfoRequest', 'magic, type, address, crc')
InfoRequestRegexp = 'B'*INFO_REQUEST_LENGTH

ConfigResponse = namedtuple('ConfigResponse', 'magic, type, old_address, new_address, crc')
ConfigResponseRegexp = 'B'*CONFIG_RESPONSE_LENGTH
ConfigRequest = namedtuple('ConfigRequest', 'magic, type, old_address, new_address, crc')
ConfigRequestRegexp = 'B'*CONFIG_REQUEST_LENGTH

VelocityResponse = namedtuple('VelocityResponse', 'magic, type, address, velocity, crc')
VelocityResponseRegexp = 'B'*VELOCITY_RESPONSE_LENGTH
VelocityRequest = namedtuple('VelocityRequest', 'magic, type, address, velocity, crc')
VelocityRequestRegexp = 'BBBbB'

ErrorResponse = namedtuple('ErrorResponse', 'magic, type, address, error_type, crc')
ErrorResponseRegexp = 'B'*ERROR_LENGTH


def calc_crc(message):
    result = 0x00
    for i in message:
        result ^= i
    return result & 0xff


def request_info(port, address=None):
    if address is not None:
        data = {'magic': MAGIC, 'type': INFO_TYPE, 'address': address}
    else:
        data = {'magic': MAGIC, 'type': FORCE_INFO_TYPE, 'address': 0x00}
    request = InfoRequest(*data.values(), calc_crc(data.values()))
    port.write(struct.pack(InfoRequestRegexp, *request))

    raw_response = port.read(INFO_RESPONSE_LENGTH)
    response = ErrorResponse._make(struct.unpack(ErrorResponseRegexp, raw_response))
    if response.type == ERROR_TYPE:
        print(response)
    else:
        response = VelocityResponse._make(struct.unpack(InfoResponseRegexp, raw_response))
        print(response)


def request_velocity(port, velocity=0x00, address=None):
    if address is None:
        data = {'magic': MAGIC, 'type': FORCE_VELOCITY_TYPE, 'address': 0x00, 'velocity': velocity}
    else:
        data = {'magic': MAGIC, 'type': VELOCITY_TYPE, 'address': address, 'velocity': velocity}
    request = VelocityRequest(*data.values(), calc_crc(data.values()))
    print(struct.pack(VelocityRequestRegexp, *request))
    port.write(struct.pack(VelocityRequestRegexp, *request))

    raw_response = port.read(VELOCITY_RESPONSE_LENGTH)

    if raw_response[1] == ERROR_TYPE:
        response = ErrorResponse._make(struct.unpack(ErrorResponseRegexp, raw_response))
        print(response)
    else:
        response = InfoResponse._make(struct.unpack(VelocityResponseRegexp, raw_response))
        print(response)


def request_config(port, velocity=0x00, old_address=None, new_address=0x00):
    if old_address is None:
        data = {'magic': MAGIC, 'type': FORCE_CONFIG_TYPE, 'old_address': 0x00, 'new_address': new_address}
    else:
        data = {'magic': MAGIC, 'type': CONFIG_TYPE, 'old_address': old_address, 'new_address': new_address}
    request = ConfigRequest(*data.values(), calc_crc(data.values()))
    print(struct.pack(ConfigRequestRegexp, *request))
    port.write(struct.pack(ConfigRequestRegexp, *request))

    raw_response = port.read(CONFIG_RESPONSE_LENGTH)

    if raw_response[1] == ERROR_TYPE:
        response = ErrorResponse._make(struct.unpack(ErrorResponseRegexp, raw_response))
        print(response)
    else:
        response = ConfigResponse._make(struct.unpack(VelocityResponseRegexp, raw_response))
        print(response)


def main():
    port = serial.Serial('/dev/ttyUSB0', timeout=10)
    # port = 0
    if len(sys.argv) > 1:
        if sys.argv[1] == 'info':
            if len(sys.argv) > 2:
                request_info(port, address=int(sys.argv[2], 0))
            else:
                request_info(port)
    else:
        print("Not enough params!")
    port.close()


if __name__ == '__main__':
    main()
