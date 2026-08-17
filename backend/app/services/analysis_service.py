import numpy as np

from backend.app.services.simulation_service import simular


def altura_a_fuerza(H, alpha=1000.0):
    """
    Relación simplificada provisional:

        F0 = alpha * H

    NOTA:
        Esta relación todavía no representa el modelo hidrodinámico
        definitivo de MARVOLT-PA. Se mantiene por compatibilidad
        con la simulación inicial.
    """
    return alpha * H


def analizar_respuesta(m, c, k, w, alturas):
    """
    Analiza la respuesta de la boya para diferentes alturas de ola.

    Returns:
        ndarray con columnas:
        [altura_de_ola, desplazamiento_maximo]
    """

    resultados = []

    for H in alturas:
        F0 = altura_a_fuerza(H)

        t, x, v = simular(
            m=m,
            c=c,
            k=k,
            F0=F0,
            w=w
        )

        inicio_estacionario = int(len(x) * 0.7)
        x_estable = x[inicio_estacionario:]

        xmax = np.max(np.abs(x_estable))

        resultados.append((H, xmax))

    return np.array(resultados)