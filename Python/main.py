import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from config_og_variabler.config import t
from config_og_variabler.plottVariabler import ax1, ax2, ax3, ax4, ax5, ax6, fig
from config_og_variabler.configureAxes import configureAxes
import buttons.resetButton
import buttons.resetEspButton
import buttons.pauseButton
import buttons.CSVButton
from updateFrame import updateFrame
import time
import tcpServer2 as tcp
import fetchData as fd
import config_og_variabler.config as config



def main():


    fd.preProcess()
    tcp.establish_all_connections()
    configureAxes(ax1, ax2, ax3, ax4, ax5, ax6, t)
    config.ani = FuncAnimation(fig, updateFrame, interval=200, blit=False,cache_frame_data=False)
    _ = config.ani  # Bare for å ikke få advarsel om at "ani blir ikke brukt"
    # Vis figuren først
    plt.show(block=False)

    # Vent et kort øyeblikk for å gi GUI tid til å starte
    plt.pause(0.001)

    # Nå kan vi trygt stoppe animasjonen
    config.ani.event_source.stop()  # type: ignore[attr-defined]
    config.paused = True
    # Eventuelt blokk videre etterpå
    plt.show()


if __name__ == "__main__":
    main()
