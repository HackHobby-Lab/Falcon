import can
import struct

def decode_gps_data(data):
    """ Decodes GPS data from CAN frame """
    latitude = struct.unpack('f', data[0:4])[0]  # Extract 4-byte float
    longitude = struct.unpack('f', data[4:8])[0]  # Extract 4-byte float
    print(f"GPS Data -> Latitude: {latitude:.6f}, Longitude: {longitude:.6f}")

def decode_alt_speed_data(data):
    """ Decodes Altitude and Speed from CAN frame """
    altitude = struct.unpack('H', data[0:2])[0]  # Extract 2-byte unsigned int
    speed = struct.unpack('H', data[2:4])[0]  # Extract 2-byte unsigned int
    print(f"GPS Data -> Altitude: {altitude} meters, Speed: {speed} km/h")

def decode_magnetometer_data(data):
    """ Decodes Azimuth and Bearing from CAN frame """
    azimuth = struct.unpack('H', data[0:2])[0]  # Extract 2-byte unsigned int
    bearing = struct.unpack('H', data[2:4])[0]  # Extract 2-byte unsigned int
    print(f"Magnetometer Data -> Azimuth: {azimuth}°, Bearing: {bearing}°")

def decode_xyz_data(data):
    """ Decodes Magnetometer X, Y, Z values from CAN frame """
    x = struct.unpack('h', data[0:2])[0]  # Extract 2-byte signed int
    y = struct.unpack('h', data[2:4])[0]  # Extract 2-byte signed int
    z = struct.unpack('h', data[4:6])[0]  # Extract 2-byte signed int
    print(f"Magnetometer Data -> X: {x}, Y: {y}, Z: {z}")

def receive_can_data():
    try:
        # Create a CAN bus instance (adjust 'can0' if using different interface)
        bus = can.interface.Bus(channel='can0', bustype='socketcan')

        print("Listening for CAN messages...")
        while True:
            message = bus.recv()  # Wait for a message
            if message:
                print(f"Received CAN message: ID={message.arbitration_id:X}, Data={message.data.hex()}")

                # Decode messages based on their ID
                if message.arbitration_id == 0x100:
                    decode_gps_data(message.data)
                elif message.arbitration_id == 0x101:
                    decode_alt_speed_data(message.data)
                elif message.arbitration_id == 0x200:
                    decode_magnetometer_data(message.data)
                elif message.arbitration_id == 0x201:
                    decode_xyz_data(message.data)

    except KeyboardInterrupt:
        print("\nStopping CAN receiver.")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    receive_can_data()
