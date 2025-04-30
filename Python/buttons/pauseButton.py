from matplotlib.widgets import Button
import config_og_variabler.plottVariabler as pV
import config_og_variabler.config as config



def toggle_pause(event):
    if config.paused:
        config.ani.event_source.start() # type: ignore[attr-defined]
        config.paused = False
        print("Startet simuleringen")
    else:
        config.ani.event_source.stop() # type: ignore[attr-defined]
        config.paused = True
        print("Pauset simuleringen")


pause_ax = pV.fig.add_axes([0.35,0.01,0.1,0.05])
pause_button = Button(pause_ax, "Start/Stop")
pause_button.on_clicked(toggle_pause)