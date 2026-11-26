import numpy as np


def modelo_boya(t, y, m, c, k, F0, w):
    """Modelo SDOF masa-resorte-amortiguador de la boya.

    y[0] = x: desplazamiento vertical [m]
    y[1] = v: velocidad vertical [m/s]

    m: masa introducida al modelo [kg]. En las corridas preliminares
       actuales se usa la masa mecánica seca medida del conjunto móvil
       (0.19634 kg); la masa añadida del agua aún no está caracterizada.
    c: amortiguamiento equivalente [N*s/m], provisional.
    k: rigidez equivalente [N/m], provisional.
    F0: amplitud de fuerza excitadora [N], provisional.
    w: frecuencia angular de excitación [rad/s].

    Esta función resuelve la ecuación propuesta; no calibra sus parámetros.
    """
    if m <= 0:
        raise ValueError("La masa del modelo debe ser mayor que cero.")

    x, v = y
    dxdt = v
    dvdt = (F0 * np.sin(w * t) - c * v - k * x) / m
    return [dxdt, dvdt]