#!python
#
#  FreeRTOS STM32 Reference Integration
#
#  Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy of
#  this software and associated documentation files (the "Software"), to deal in
#  the Software without restriction, including without limitation the rights to
#  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
#  the Software, and to permit persons to whom the Software is furnished to do so,
#  subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
#  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
#  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
#  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
#  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#  https://www.FreeRTOS.org
#  https://github.com/FreeRTOS
#
#
import argparse
import io
import logging
from time import monotonic

import serial
import serial.tools.list_ports

logger = logging.getLogger()


class TargetDevice:
    _running_config = None
    _staged_config = None
    _error_bstr = (b"error", b"Error", b"ERROR", b"<ERR>")
    _timeout = 2.0

    class ReadbackError(Exception):
        """Raised when the command readback received from the target does not match the command sent."""

        pass

    class ResponseTimeout(Exception):
        """Raised when the target fails to respond within the alloted timeout"""

        pass

    class TargetError(Exception):
        """Raised when the target returns an error message"""

        pass

    def __init__(self, device, baud):
        """Connect to a target device"""
        self.ser = serial.Serial(device, baud, timeout=0.1, rtscts=False)
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()
        self.sio = io.BufferedRWPair(self.ser, self.ser)
        self.sio._CHUNK_SIZE = 2
        self._sync()

    def _sync(self):
        """Send Control+C (0x03) control code to clear the current line."""
        self.sio.write(b"\x03")
        self.sio.flush()
        self._read_response()

    def _send_cmd(self, *args, timeout=_timeout):
        """Send a single command to the target and return True if the readback matches."""
        cmd = b" ".join(args)
        cmdstr = cmd + b"\r\n"

        self.sio.write(cmdstr)
        logging.debug("TX: {} (_send_cmd)".format(cmdstr))

        self.sio.flush()

        timeoutTime = monotonic() + timeout

        cmd_readback = self.sio.readline()
        while timeoutTime > monotonic():
            if len(cmd_readback) > 0:
                break
            cmd_readback = self.sio.readline()

        logging.debug("RX: {} (_send_cmd)".format(cmd_readback))
        if cmd not in cmd_readback:
            raise TargetDevice.ReadbackError()

    def _read_response(self, timeout=_timeout):
        """Read the response to a command"""
        response = []
        errorFlag = False
        timeoutFlag = True

        timeoutTime = monotonic() + timeout

        while timeoutTime > monotonic():
            line = self.sio.readline()

            if len(line) == 0:
                continue

            logging.debug("RX: {} (_read_response)".format(line))

            if b"> " == line:
                timeoutFlag = False
                break
            elif any(errStr in line for errStr in self._error_bstr):
                errorFlag = True

            response.append(line)

        if errorFlag:
            raise TargetDevice.TargetError()
        elif timeoutFlag:
            raise TargetDevice.ResponseTimeout()

        return response

    def _read_pem(self, timeout=_timeout):
        """Read a pem file from a command response and return it as a byte string"""
        beginStr = b"-----BEGIN "
        endStr = b"-----END "

        pem = []

        readingPemFlag = False
        errorFlag = False
        timeoutFlag = True

        timeoutTime = monotonic() + timeout

        while monotonic() < timeoutTime:
            line = self.sio.readline()

            if len(line) == 0:
                continue

            logging.debug("RX: {} (_read_pem)".format(line))

            # normalize line endings
            line = line.replace(b"\r\n", b"\n")

            if b"> " in line:
                if line == b"> ":
                    errorFlag = True
                    timeoutFlag = False
                    break
                else:
                    line = line.replace(b"> ", b"")

            if readingPemFlag:
                pem.append(line)
                if endStr in line:
                    timeoutFlag = False
                    break
            elif beginStr in line:
                readingPemFlag = True
                pem.append(line)
            elif any(errStr in line for errStr in self._error_bstr):
                errorFlag = True
                break

        if errorFlag:
            raise TargetDevice.TargetError()
        elif timeoutFlag:
            raise TargetDevice.ResponseTimeout()
        # Handle any lines in output after the pem footer
        else:
            self._read_response()

        combined = b"".join(pem)
        return combined

    def _write_pem(self, pem):
        """Write a pem bytestring to the target and raise an error if the readback does not match."""
        # Remove any existing CR
        pem = pem.replace(b"\r\n", b"\n")
        for line in pem.split(b"\n"):
            line = line + b"\r\n"
            logging.debug("TX: {} (_write_pem)".format(line))
            self.sio.write(line)

        self.sio.write(b"\r\n\r\n")
        self.sio.flush()

        pemReadback = self._read_pem(timeout=5)

        if pem != pemReadback:
            raise TargetDevice.ReadbackError(
                "Readback does not match provided pem file."
            )

    def write_pubkey(self, pub_key, label=None):
        """Write a public key in pem format to the specified label."""
        if label:
            self._send_cmd(b"pki import key", bytes(label, "ascii"))
        else:
            self._send_cmd(b"pki import key")

        self._write_pem(pub_key)


def find_serial_port(usbVendorId=0x0483, usbProductId=0x374E):
    ports = serial.tools.list_ports.comports()
    matches = []
    device = None

    for port in ports:
        attrs = dir(port)
        if (
            "vid" in attrs
            and port.vid
            and "pid" in attrs
            and port.pid
            and "device" in attrs
            and port.device
        ):
            logging.debug(
                "Found device: {} with Manufacturer: {}, Vendor ID: {}, and Product ID: {}".format(
                    port.device, port.manufacturer, hex(port.vid), hex(port.pid)
                )
            )
            if port.vid == usbVendorId and port.pid == usbProductId:
                matches.append(port.device)
    # default to an empty string if no match was found.
    if len(matches) > 0:
        device = matches[0]
    return device


def provision_ota(target, pub_key):
    target.write_pubkey(pub_key, label="ota_signer_pub")


def process_args():
    parser = argparse.ArgumentParser(argument_default=argparse.SUPPRESS)

    parser.add_argument("-v", "--verbose", action="store_true")

    # Default to stlink vid/pid if only one is connected, otherwise error.
    parser.add_argument("-d", "--device", type=str)

    # Key filename
    parser.add_argument("-k", "--key", type=str)

    return parser.parse_args()


def main():
    args = process_args()

    if "verbose" in args:
        logging.getLogger().setLevel(logging.DEBUG)

    devpath = find_serial_port()
    if "device" in args:
        devpath = args.device

    if not devpath or len(devpath) == 0:
        logging.error(
            'Target device path could not be determined automatically. Please call this script with the "device" argument'
        )
        raise SystemExit
    else:
        print("Target device path: {}".format(devpath))

    if "key" in args:
        key_filename = args.key
    else:
        logging.error(
            'Key not specified. Please call this script with the "--key" argument'
        )
        raise SystemExit

    print("Reading public key from file...")
    key_file = open(key_filename, "rb")
    key = key_file.read()
    key_file.close()

    print("Connecting to target...")

    target = TargetDevice(devpath, 115200)

    print("Provisioning OTA public key...")
    provision_ota(target, key)
    print("Provisioning process complete.")


if __name__ == "__main__":
    logging.basicConfig(
        level=logging.INFO,
        format="[ %(levelname)s ] %(message)s (%(filename)s:%(funcName)s)",
    )
    main()
