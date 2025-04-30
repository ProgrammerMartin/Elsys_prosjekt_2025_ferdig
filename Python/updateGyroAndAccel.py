import rotateVector as g
from config_og_variabler.sensorData import adata, vdata, vdata_abs, sdata, initial_direction
from config_og_variabler.plottVariabler import ax2, gyro_vector, indicator_ax
from config_og_variabler.config import t
import numpy as np
import time
from collections import deque
import config_og_variabler.sensorData as sensorData

_ = adata  # Dette gjør at lintern forstår at adata faktisk brukes
_ = vdata
_ = vdata_abs
_ = sdata
_ = gyro_vector

k = 21
tid = 0
nettoppSlag = False

# Sett ønsket vindusstørrelse
window_size = 2

# Lager buffer for akselerasjonsmålingene
accelx_window = deque(maxlen=window_size)
accely_window = deque(maxlen=window_size)
accelz_window = deque(maxlen=window_size)


def updateGyroAndAccel(accelx, accely, accelz, gyrox, gyroy, gyroz, current_sim_time):
    global gyro_vector, adata, vdata, vdata_abs, sdata, a_gravityVector, tid, nettoppSlag    


    if gyrox is not None:
        g.changeRotationMatrix(k*gyrox, k*gyroy, k*gyroz, t)  # Justerer den globale rotasjonsmatrisen R utfra nye gyroskopmålinger
        retning = g.getGyroDirection()  # Gir retningen til mpu-en
        sensorData.directionData.append(retning)

        # gyro_vector.remove()  # Fjerner den tidligere retningsvektoren
        # gyro_vector = ax2.quiver(0, 0, 0, retning[0], retning[1], retning[2], color="red")  # Tegner en ny retningsvektor ut fra retning-arrayen

    if accelx is not None:
        # print(f"Raw accel: {accelx, accely, accelz}")
        # Legg til ny måling i vinduet
        accelx_window.append(accelx)
        accely_window.append(accely)
        accelz_window.append(accelz)
        
         # Sjekk om vi har nok målinger til å begynne å glatte
        if len(accelx_window) == window_size:
            smoothed_accelx = np.mean(list(accelx_window))
            smoothed_accely = np.mean(list(accely_window))
            smoothed_accelz = np.mean(list(accelz_window))
            # print(f"Smoothed accel: {smoothed_accelx, smoothed_accely, smoothed_accelz}")
            accelx_mpu_coordinates_corrected = smoothed_accelx - 9.81*2*g.getProjectionOnZaxis(np.array([1, 0, 0]))
            accely_mpu_coordinates_corrected = smoothed_accely - 9.81*2*g.getProjectionOnZaxis(np.array([0, 1, 0]))
            accelz_mpu_coordinates_corrected = smoothed_accelz - 9.81*2*g.getProjectionOnZaxis(np.array([0, 0, 1]))
        else:
            accelx_mpu_coordinates_corrected = accelx - 9.81*2*g.getProjectionOnZaxis(np.array([1, 0, 0]))
            accely_mpu_coordinates_corrected = accely - 9.81*2*g.getProjectionOnZaxis(np.array([0, 1, 0]))
            accelz_mpu_coordinates_corrected = accelz - 9.81*2*g.getProjectionOnZaxis(np.array([0, 0, 1]))

        gravity_mpu_coordinate_system = g.getGravityMpuCoordinates()

        accel_mpu_coordinate_system = np.array([accelx_mpu_coordinates_corrected, accely_mpu_coordinates_corrected, accelz_mpu_coordinates_corrected])

        a_in_real_world = g.getARealWorld(accel_mpu_coordinate_system, gravity_mpu_coordinate_system)
        # print(f"a_in_real_world = {a_in_real_world}")
        # Regner ut lengden på akselerasjonen, for plotting
        accel_magnitude = np.sqrt(a_in_real_world[0]**2 + a_in_real_world[1]**2 + a_in_real_world[2]**2)
        # if len(adata) > 0 and ((abs(accel_magnitude - adata[-1]) > 1) and (not nettoppSlag)):
        #     tid = current_sim_time
            
        #     if (accel_magnitude - adata[-1] > 10):
        #         print("VELDIG HARDT SLAG!")
        #         indicator_ax.set_facecolor("#FF0000")
        #     else:
        #         if (accel_magnitude - adata[-1] > 7):
        #             print("HARDT SLAG!")
        #             indicator_ax.set_facecolor("#FD6262")
        #         else:
        #             print("SLAG!")
        #             indicator_ax.set_facecolor("#FFC6C6")
        #     nettoppSlag = True
        # else:
        #     if current_sim_time - tid > 0.3:
        #         indicator_ax.set_facecolor("grey")
        #         nettoppSlag = False

        # Oppdaterer listen med akselerasjons-størrelse-data
        adata.append(accel_magnitude)

        # Oppdaterer listen med fartdata for ulike retninger
        vdata[:-1], vdata[-1] = vdata[1:], (
            vdata[-1][0] + a_in_real_world[0] * t,
            vdata[-1][1] + a_in_real_world[1] * t,
            vdata[-1][2] + a_in_real_world[2] * t
        )

        # Oppdaterer lengden av fartvektoren, total_speed, og legger til i listen som inneholder denne daten. Til plott.
        total_speed = np.sqrt(
            vdata[-1][0]**2 +
            vdata[-1][1]**2 +
            vdata[-1][2]**2)
        vdata_abs.append(total_speed)

        # Oppdaterer posisjon.
        if len(sdata) == 0:
            # Hvis sdata er tom, legg til startpunkt (0, 0, 0)
            sdata.append((0.0, 0.0, 0.0))
        else:
            new_pos = (
                sdata[-1][0] + vdata[-1][0] * t,
                sdata[-1][1] + vdata[-1][1] * t,
                sdata[-1][2] + vdata[-1][2] * t,
            )
            sdata.append(new_pos)

def updateGyro(gyrox, gyroy, gyroz,):
    global gyro_vector, adata, vdata, vdata_abs, sdata, a_gravityVector, tid, nettoppSlag  

    if gyrox is not None:
        g.changeRotationMatrix(k*gyrox, k*gyroy, k*gyroz, t)  # Justerer den globale rotasjonsmatrisen R utfra nye gyroskopmålinger
        retning = g.getGyroDirection()  # Gir retningen til mpu-en

        gyro_vector.remove()  # Fjerner den tidligere retningsvektoren
        gyro_vector = ax2.quiver(0, 0, 0, retning[0], retning[1], retning[2], color="red")  # Tegner en ny retningsvektor ut fra retning-arrayen

def resetGyro():
    global gyro_vector
    g.resetR()
    try:
        gyro_vector.remove()  # Remove previous gyro vector plot
        gyro_vector = ax2.quiver(0, 0, 0, initial_direction[0], initial_direction[1], initial_direction[2], color="red")
    except ValueError:
        pass
    
