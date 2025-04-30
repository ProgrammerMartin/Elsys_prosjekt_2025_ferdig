from matplotlib.widgets import Button
from config_og_variabler.plottVariabler import gyro_vector, fig
from config_og_variabler.sensorData import adata, vdata, vdata_abs, sdata
import config_og_variabler.config as fV
from updateGyroAndAccel import resetGyro
import tcpServer2 as tcp
import config_og_variabler.sensorData as sensorData
_ = gyro_vector
_ = adata
_ = vdata
_ = vdata_abs
_ = sdata


def resetEsp(event):
    print("resetESP() kalt")
    global adata, vdata, vdata_abs, sdata, gyro_vector

    try:
        tcp.send_reset_command()
        tcp.establish_all_connections()
    except Exception as e:
        print(f"Feil ved sending: {e}")


reset_ax = fig.add_axes([0.85, 0.01, 0.1, 0.05])
reset_esp_button = Button(reset_ax, 'Reset ESP', color='#F23C3C')
reset_esp_button.on_clicked(resetEsp)
