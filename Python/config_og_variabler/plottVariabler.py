import matplotlib.pyplot as plt
from config_og_variabler.sensorData import adata, vdata_abs, initial_direction, piezoData
import numpy as np


# Oppretter en figur
fig = plt.figure(figsize=(14, 8))

# Oppretter subplots til figuren individuelt
ax1 = fig.add_subplot(2, 3, 1)  # 2D-plot øverst til venstre
ax3 = fig.add_subplot(2, 3, 2)  # 2D-plot øverst til høyre
ax4 = fig.add_subplot(2, 3, 3, projection='3d')  # 3D-plot nederst til venstre
ax2 = fig.add_subplot(2, 3, 6, projection='3d')  # 3D-plot nederst til høyre
ax5 = fig.add_subplot(2, 3, 4)
ax6 = fig.add_subplot(2, 3, 5)

ax3.set_visible(False)

plt.subplots_adjust(hspace=0.4, wspace=0.6)  # Juster avstander

# Linjene som skal plottes
(line1,) = ax1.plot([], [], "r-", label="Akselerasjon")
(line3,) = ax3.plot([], [], "g-", label="Fart")
(line4,) = ax4.plot([], [], [], "b-", label="Posisjon", linewidth=3)
(line5,) = ax5.plot([], [], "r-", label="Frekvenser")
(line6,) = ax6.plot([], [], "r", label="piezoData")

# Initialize 3D gyro vector plot
gyro_vector = ax2.quiver(0, 0, 0, initial_direction[0], initial_direction[1], initial_direction[2], color="red")

# rotation_axis_vector = ax2.quiver(0, 0, 0, 0, 0, 1, color="blue")

# increment = 1

# Start med identitetsmatrise (MPU starter i en kjent orientering)
R = np.eye(3)

# Gravitasjons-vektor i den virkelige verden
gravity_vector = np.array([0, 0, -9.81])

# Parametervariabel
t = np.linspace(-10, 10, 100)  # Juster området og oppløsning etter behov

# Koordinater
x = t
y = t
z = t

# Tegn linjen i subplot ax4
# ax4.plot3D(x, y, z, color='red', label='r(t) = (t, t, t)')


# Legg til en boks øverst i figuren (manuelt plassert Axes-område)

updateFftPlot = False

# Legg til en boks øverst i figuren (manuelt plassert Axes-område)
indicator_ax = fig.add_axes([0.415, 0.7, 0.2, 0.05])  # [left, bottom, width, height] i figur-koordinater
indicator_ax.set_xticks([])
indicator_ax.set_yticks([])
indicator_ax.set_facecolor("gray")  # Startfarge, endres senere
indicator_ax.set_title("Slagindikator", fontsize=10)