import socket
import threading
import time
import struct

HOST = '0.0.0.0'
IP_ESP32 = '192.168.167.191'

PORTS = {
    'START': 9000,
    'MPU': 5000,
    'FFT': 7000,
    'PIEZO': 8000,
    'RESET': 6000
}

connections = {}

STRUCT_FORMAT_MPU = '<ffffffH'  # liten endian: 6 floats og 1 unsigned short
STRUCT_FORMAT_FFT = '<H' + 'H'*130
STRUCT_FORMAT_PIEZO = 'HH'


STRUCT_SIZE_MPU = struct.calcsize(STRUCT_FORMAT_MPU)
STRUCT_SIZE_FFT = struct.calcsize(STRUCT_FORMAT_FFT)
STRUCT_SIZE_PIEZO = struct.calcsize(STRUCT_FORMAT_PIEZO)

MPUBuffer = []
FFTBuffer = []
PiezoBuffer = []


def establish_all_connections():
    for name, port in PORTS.items():
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, port))
        s.listen(1)
        print(f"[{name}] Lytter på port {port} – venter på tilkobling...")
        conn, addr = s.accept()
        print(f"[{name}] Tilkoblet fra {addr}")
        connections[name] = conn



def send_start_command():
    conn = connections.get('START')
    if conn:
        conn.sendall(b'START\n')
        print("Sendte start-kommando til ESP")
    else:
        print("Ikke tilkoblet på start-porten!")

def send_reset_command():
    conn = connections.get('RESET')
    if conn:
        conn.sendall(b'RESTART\n')
        print("Sendte reset-kommando til ESP")
    else:
        print("Ikke tilkoblet på reset-porten!")


def receive_all_data(port_name, struct_format, struct_size, label):
    buffer = b''
    decoded = []

    conn = connections.get(port_name)
    if not conn:
        print(f"[{label}] Ikke koblet til {port_name}-porten.")
        return []

    conn.settimeout(1.0)  # kort timeout for å sjekke periodisk

    last_data_time = time.time()
    max_wait_time = 10.0

    try:
        while True:
            if (time.time() - last_data_time) > max_wait_time:
                print(f"[{label}] Ingen data på 20 sekunder – avslutter.")
                break

            try:
                data = conn.recv(1024)

                if not data:
                    print(f"[{label}] Forbindelse brutt.")
                    break

                last_data_time = time.time()

                if b'END' in data:
                    print(f"[{label}] Mottatt END – ferdig!")
                    break

                print(f"[DEBUG][{label}] Fikk {len(data)} byte")
                buffer += data

                while len(buffer) >= struct_size:
                    chunk = buffer[:struct_size]
                    try:
                        unpacked = struct.unpack(struct_format, chunk)
                        decoded.append(unpacked)
                    except struct.error as e:
                        print(f"[{label}] Feil ved dekoding: {e}")
                    buffer = buffer[struct_size:]

            except socket.timeout:
                print(f"[{label}] Timeout, venter...")

    except Exception as e:
        print(f"[{label}] Feil under mottak: {e}")

    return decoded

def receive_all_mpu_data():
    return receive_all_data('MPU', STRUCT_FORMAT_MPU, STRUCT_SIZE_MPU, 'MPU')

def receive_all_fft_data():
    return receive_all_data('FFT', STRUCT_FORMAT_FFT, STRUCT_SIZE_FFT, 'FFT')

def receive_all_piezo_data():
    return receive_all_data('PIEZO', STRUCT_FORMAT_PIEZO, STRUCT_SIZE_PIEZO, 'Piezo')
