import serial


def main():
    ser = serial.Serial('/dev/ttyUSB0')  # open serial port
    print(ser.name)         # check which port was really used
    ser.write(b'hello')     # write a string
    a = ser.read(100)
    print(a)
    ser.close()             # close port


if __name__ == "__main__":
    main()
