import time
import numpy as np
from config_og_variabler.plottVariabler import line1, line3, line4, line5, line6, gyro_vector
import config_og_variabler.plottVariabler as pV
import config_og_variabler.sensorData as sensorData
from config_og_variabler.sensorData import adata, vdata_abs, sdata, piezoData
import config_og_variabler.config as config
import updateGyroAndAccel as uGA

force_text = pV.indicator_ax.text(
    0.5, 0.5, "",  # Start with empty string
    color='white', ha='center', va='center', fontsize=12
)

UPDATE_EVERY_N = 3

def updateFrame(frame):
    
    real_frame = config.frame_number
    config.frame_number += 1

    if real_frame >= len(sensorData.time_fft):
        print("Simulering Ferdig")
        config.ani.event_source.stop()  # type: ignore[attr-defined]
        config.paused = True
        return


    current_sim_time = sensorData.time_fft[real_frame]

    line5.set_xdata(sensorData.freqs)
    line5.set_ydata(sensorData.fftData[real_frame])

    mask = sensorData.time_piezo <= current_sim_time
    line6.set_xdata(sensorData.time_piezo[mask])
    line6.set_ydata(sensorData.piezoData[mask])

    if (sensorData.piezoData[mask] > 16).any():
        print("HARDT SLAG!")
        pV.indicator_ax.set_facecolor("red")
        #nettoppSlag = True
    elif (sensorData.piezoData[mask] > 8).any():
        print("SLAG")
        pV.indicator_ax.set_facecolor("orange")
        #nettoppSlag = True
    else:
        print("Ingen slag")
        pV.indicator_ax.set_facecolor("grey")
        #nettoppSlag = False

    maxForce = 1
    masked_piezo = sensorData.piezoData[mask]
    if len(masked_piezo) > 0:
        max_adc = np.max(masked_piezo)
        maxForce = max_adc  # ADC → Voltage → Force (your formula)

    # Update the text already created
    #pv.indicator_ax.updatetext( f"{maxForce:.2f} Newton")
    force_text.set_text(f"{maxForce:.2f} Newton")


    indices = np.where(sensorData.time_mpu > current_sim_time)[0]
    if len(indices) == 0:
        i_mpu = len(sensorData.time_mpu)
    else:
        i_mpu = indices[0]

    i_mpu = min(i_mpu, len(sensorData.directionData) - 1)

    try:
        pV.gyro_vector.remove()
    except ValueError:
        pass  # Den var allerede fjernet

    # pV.gyro_vector.remove()  # Fjerner den tidligere retningsvektoren
    pV.gyro_vector = pV.ax2.quiver(0, 0, 0, sensorData.directionData[i_mpu][0], sensorData.directionData[i_mpu][1], sensorData.directionData[i_mpu][2], color="red")  # Tegner en ny retningsvektor ut fra retning-arrayen

    # line1
    
    line1.set_xdata(sensorData.time_mpu[:i_mpu])
    line1.set_ydata(sensorData.adata[:i_mpu])

    # line3
    
    line3.set_xdata(sensorData.time_mpu[:i_mpu])
    line3.set_ydata(sensorData.vdata_abs[:i_mpu])

    sdata_np = np.array(sdata)

    line4.set_data(sdata_np[:i_mpu, 0], sdata_np[:i_mpu, 1])
    line4.set_3d_properties(sdata_np[:i_mpu, 2])

    return line1, line3, line4, gyro_vector, line5, line6


        # === Oppdater plott ===
        # min_len = min(len(sensorData.time_buffer), len(sensorData.adata))
        # line1.set_ydata(adata[:min_len])
        # line1.set_xdata(sensorData.time_buffer[:min_len])

        # min_len = min(len(sensorData.time_buffer), len(sensorData.vdata_abs))
        # line3.set_ydata(vdata_abs[:min_len])
        # line3.set_xdata(sensorData.time_buffer[:min_len])

        # line4.set_data(sdata[:, 0], sdata[:, 1])
        # line4.set_3d_properties(sdata[:, 2])
