import numpy as np


def modelo_boya(t, y, m, c, k, F0, w):
    """
    Modelo masa-resorte-amortiguador de la boya.

    Variables de estado:
        y[0] = x  -> desplazamiento [m]
        y[1] = v  -> velocidad [m/s]

    Parámetros:
        m  -> masa [kg]
        c  -> amortiguamiento [N·s/m]
        k  -> rigidez [N/m]
        F0 -> amplitud de la fuerza de excitación [N]
        w  -> frecuencia angular [rad/s]
    """
    x, v = y

    dxdt = v
    dvdt = (F0 * np.sin(w * t) - c * v - k * x) / m

    return [dxdt, dvdt]