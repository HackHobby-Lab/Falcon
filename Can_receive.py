import can
import struct
import time

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

def decode_obstacle_data(data):
    """ Decodes Obstacle Detection Data from CAN frame """
    try:
        message = data.decode('utf-8').strip('\x00')  # Convert bytes to string, remove null terminators
        print(f"Obstacle Detection -> {message}")
    except Exception as e:
        print(f"Error decoding obstacle message: {e}")

def send_status(bus):
    """ Sends this node's own status in a CAN message (ID 0x203) """
    status_byte = 1  # 1 = Online
    message = can.Message(arbitration_id=0x203, data=[status_byte], is_extended_id=False)

    try:
        bus.send(message)
        print(f"Sent status message: ID=0x203, Data={status_byte:08b} (Online)")
    except can.CanError:
        print("Failed to send CAN message.")

def receive_can_data():
    try:
        # Create a CAN bus instance (adjust 'can0' if using different interface)
        bus = can.interface.Bus(channel='can0', bustype='socketcan')

        print("Listening for CAN messages...")
        while True:
            message = bus.recv(timeout=1)  # Wait for a message, timeout to send status
            if message:
                print(f"Received CAN message: ID={message.arbitration_id:X}, Data={message.data.hex()}")

                # Decode messages based on their ID
                if message.arbitration_id == 0x100:
                    decode_gps_data(message.data)
                # elif message.arbitration_id == 0x109:
                #     decode_alt_speed_data(message.data)
                elif message.arbitration_id == 0x101:
                    decode_obstacle_data(message.data)
                elif message.arbitration_id == 0x200:
                    decode_magnetometer_data(message.data)
                elif message.arbitration_id == 0x201:
                    decode_xyz_data(message.data)

            # Send this node's status every 2 seconds
            send_status(bus)
            time.sleep(1)

    except KeyboardInterrupt:
        print("\nStopping CAN receiver.")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    receive_can_data()
