import numpy as np

def modelo_boya(t, y, m, c, k, F0, w):
    """
    Sistema de ecuaciones:
    y[0] = x (desplazamiento)
    y[1] = v (velocidad)
    """
    x, v = y

    dxdt = v
    dvdt = (F0 * np.sin(w * t) - c * v - k * x) / m

    return [dxdt, dvdt]