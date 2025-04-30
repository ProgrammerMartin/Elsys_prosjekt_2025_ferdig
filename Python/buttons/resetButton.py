from matplotlib.widgets import Button
import config_og_variabler.plottVariabler as pV
from config_og_variabler.sensorData import adata, vdata, vdata_abs, sdata
from updateGyroAndAccel import resetGyro
import config_og_variabler.sensorData as sensorData
import fetchData as fD
import config_og_variabler.config as config
_ = adata
_ = vdata
_ = vdata_abs
_ = sdata

def reset_sim(event):
    # Nullstill buffere og plottdata
    config.ani.event_source.stop() # type: ignore[attr-defined]
    sensorData.fftData.clear()
    sensorData.freqs.clear()
    sensorData.time_fft.clear()
    sensorData.piezoData.resize(0)
    sensorData.time_piezo.resize(0)
    adata.clear()
    vdata.fill(0)
    vdata_abs.clear()
    sdata.clear()
    sensorData.directionData.clear()
    resetGyro()
    config.frame_number = 0
    fD.preProcess()
    print("Simulasjonen er nullstilt!")
    config.ani.event_source.start() # type: ignore[attr-defined]
    config.paused = False

reset_ax = pV.fig.add_axes([0.65, 0.01, 0.1, 0.05])
reset_button = Button(reset_ax, "Reset Simulasjon")
reset_button.on_clicked(reset_sim)