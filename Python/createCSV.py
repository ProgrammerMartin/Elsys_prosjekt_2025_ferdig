import csv
import tcpServer2 as tcp
import shutil
import os
from datetime import datetime


def archive_old_csv_files(storage_dir="lagring"):
    # Opprett mappe hvis den ikke finnes
    os.makedirs(storage_dir, exist_ok=True)

    # Liste over CSV-filer du ønsker å arkivere
    csv_files = ["mpu_data.csv", "fft_data.csv", "piezo_data.csv"]

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")  # f.eks. 20250425_141501

    for file in csv_files:
        if os.path.exists(file):
            new_filename = f"{file.replace('.csv', '')}_{timestamp}.csv"
            destination = os.path.join(storage_dir, new_filename)
            shutil.move(file, destination)
            print(f"Flyttet {file} til {destination}")
        else:
            print(f"Fant ikke {file}, ingenting å flytte.")




def create_csv_file_mpu(filename="mpu_data.csv"):
    header = ['accel_x', 'accel_y', 'accel_z', 'gyro_x', 'gyro_y', 'gyro_z','time']
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(header)

def write_mpu_csv(data_list, filename="mpu_data.csv"):
    with open(filename, 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerows(data_list)

def create_csv_file_fft(filename="fft_data.csv"):
    header = ['time'] + [f'fft_{i}' for i in range(130)]
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(header)

def write_fft_csv(data_list, filename="fft_data.csv"):
    with open(filename, 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerows(data_list)

def create_csv_file_piezo(filename="piezo_data.csv"):
    header = ['value', 'time']
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(header)

def write_piezo_csv(data_list, filename="piezo_data.csv"):
    with open(filename, 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerows(data_list)
