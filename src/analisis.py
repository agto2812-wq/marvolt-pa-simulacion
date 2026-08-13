import numpy as np
from src.simulacion import simular

def altura_a_fuerza(H, alpha=1000):
    """
    Relación simplificada:
    F0 = alpha * H
    """
    return alpha * H

def analizar_respuesta(m, c, k, w, alturas):
    resultados = []

    for H in alturas:
        F0 = altura_a_fuerza(H)

        t, x, v = simular(m, c, k, F0, w)

        # Tomamos amplitud máxima en régimen (última parte)
        x_estable = x[int(len(x)*0.7):]
        xmax = np.max(np.abs(x_estable))

        resultados.append((H, xmax))

    return np.array(resultados)