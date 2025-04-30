import numpy as np
from config_og_variabler.sensorData import initial_direction
from config_og_variabler.plottVariabler import R, gravity_vector


def changeRotationMatrix(delta_theta_x, delta_theta_y, delta_theta_z, t):
    global R  # Beholder den totale orienteringen

    # theta = delta_theta * t. Sjekk ut om det egentlig bør gjøres om til radianer her.
    theta_x = np.radians(delta_theta_x*t)
    theta_y = np.radians(delta_theta_y*t)
    theta_z = np.radians(delta_theta_z*t)

    Rx = np.array([
        [1, 0, 0],
        [0, np.cos(theta_x), -np.sin(theta_x)],
        [0, np.sin(theta_x), np.cos(theta_x)]
    ])

    Ry = np.array([
        [np.cos(theta_y), 0, np.sin(theta_y)],
        [0, 1, 0],
        [-np.sin(theta_y), 0, np.cos(theta_y)]
    ])

    Rz = np.array([
        [np.cos(theta_z), -np.sin(theta_z), 0],
        [np.sin(theta_z), np.cos(theta_z), 0],
        [0, 0, 1]
    ])

    # Sammensatt rotasjon (i gyroskopets koordinatsystem)
    dR = Rz @ Ry @ Rx  # OBS: rekkefølgen er viktig!

    # Oppdater den totale rotasjonsmatrisen (R)
    R = R @ dR


def getGyroDirection():
    return R @ initial_direction  # Roterer start-retningen med R


def resetR():
    global R
    R = np.eye(3)  # Setter R tilbake til ingen rotasjon (Identitetsmatrisen)


def getGravityMpuCoordinates():
    return R.T @ gravity_vector  # R.T brukes for transformasjon til mpu-koordinater, mens R brukes for transformasjon til virkelighetens koordinater


def getARealWorld(accel_mpu_CS, gravity_mpu_CS):
    return R @ (accel_mpu_CS - gravity_mpu_CS)  # accel_mpu_CS - gravity_mpu_CS = akselerasjon i mpu-ens koordinatsystem
                                                # minus tyngdeaks g, slik at den skal vise 0m/s^2 i alle retninger
                                                # når i ro, uansett orientering


def getProjectionOnZaxis(vector):
    vector_rotated = R @ vector  # Roterer startvektor-aksen til der aksen (i mpu-ens koordinatsystem) er i virkeligheten
    z = np.array([0, 0, 1])
    projection = np.dot(vector_rotated, z)  # Hvor mye (fra -1 til 1) av denne aksen ligger langs virkelighetens definerte z-akse
    return projection
