import numpy as np
from config_og_variabler.config import t, toll

# Variabler for data
# time = np.linspace(-toll * t, 0, toll)

# Lister for akselerasjon, hastighet og posisjon
adata = []
vdata = np.zeros((toll, 3))
vdata_abs = []
sdata = []
directionData = []


time_mpu = np.array([])
accel_x = np.array([])
accel_y = np.array([])
accel_z = np.array([])
gyro_x = np.array([])
gyro_y = np.array([])
gyro_z = np.array([])

# Initialisering for retningsvektoren
initial_direction = np.array([0, 1, 0])

fftData = []
time_fft = []
freqs = []

piezoData = np.array([])
time_piezo = np.array([])
