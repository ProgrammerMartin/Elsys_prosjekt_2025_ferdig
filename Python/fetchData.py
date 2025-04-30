import pandas as pd
import config_og_variabler.sensorData as sensorData
import updateGyroAndAccel as uGA
import numpy as np

# ← Endre disse stiene hvis CSV-filene ligger andre steder
ACCEL_GYRO_CSV = "mpu_data.csv"
FFT_CSV = "fft_data.csv"
PIEZO_CSV = "piezo_data.csv"

# Les alle dataene inn én gang



def preProcess():
    # Legger til piezo dataen

    df_accelgyro = pd.read_csv(ACCEL_GYRO_CSV)
    df_fft = pd.read_csv(FFT_CSV)
    df_piezo = pd.read_csv(PIEZO_CSV)
    sensorData.piezoData = df_piezo["value"].to_numpy(dtype=float)
    sensorData.time_piezo = df_piezo["time"].to_numpy(dtype=float)

    sensorData.piezoData = (sensorData.piezoData / 4095) * 3.3 * 7.5

    # for i in range(0,len(sensorData.piezoData)):
    #     sensorData.piezoData[i] = (sensorData.piezoData[i] / 4095) * 3.3 * 5

    # print(sensorData.piezoData)

    # Legger til FFT dataen
    fft_cols = [col for col in df_fft.columns if col.lower().startswith("fft")]
    freqs = np.linspace(15,2000,len(fft_cols))
    sensorData.freqs = freqs.tolist()
    for _, row in df_fft.iterrows():
        # Legg til tid
        sensorData.time_fft.append(float(row["time"]))
        # Hent FFT-verdier som liste
        fft_values = row[fft_cols].tolist()
        sensorData.fftData.append(fft_values)

    # print(sensorData.fftData)

    # Legger til MPU dataen
    sensorData.time_mpu = df_accelgyro["time"].to_numpy(dtype=float)
    sensorData.accel_x = df_accelgyro["accel_x"].to_numpy(dtype=float)
    sensorData.accel_z = df_accelgyro["accel_z"].to_numpy(dtype=float)
    sensorData.accel_y = df_accelgyro["accel_y"].to_numpy(dtype=float)
    sensorData.gyro_x = df_accelgyro["gyro_x"].to_numpy(dtype=float)
    sensorData.gyro_z = df_accelgyro["gyro_z"].to_numpy(dtype=float)
    sensorData.gyro_y = df_accelgyro["gyro_y"].to_numpy(dtype=float)

    for _, row in df_accelgyro.iterrows():
        uGA.updateGyroAndAccel(row["accel_x"],row["accel_y"],row["accel_z"], row['gyro_x'], row['gyro_y'], row['gyro_z'],row["time"])

    return