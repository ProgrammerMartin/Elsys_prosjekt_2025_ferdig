from matplotlib.widgets import Button
import config_og_variabler.plottVariabler as pV
from buttons.resetButton import reset_sim
import tcpServer2 as tcp
import createCSV as csv
from dataHenting import run_sequence
import fetchData as fD
import config_og_variabler.sensorData as sensorData
from config_og_variabler.sensorData import adata, vdata, vdata_abs, sdata
import config_og_variabler.config as config
from updateGyroAndAccel import resetGyro


def henteCSV(event):
    sensorData.fftData.clear()
    sensorData.freqs.clear()
    sensorData.time_fft.clear()
    sensorData.piezoData.resize(0)
    sensorData.time_piezo.resize(0)
    adata.clear()
    vdata.fill(0)
    vdata_abs.clear()
    sdata.clear()
    resetGyro()
    config.frame_number = 0
    run_sequence()
    fD.preProcess()






CSV_ax = pV.fig.add_axes([0.15,0.01,0.1,0.05])
CSV_button = Button(CSV_ax, "Hent ny data")
CSV_button.on_clicked(henteCSV)