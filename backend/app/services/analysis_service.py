import numpy as np

from backend.app.services.simulation_service import simular


def altura_a_fuerza(H: float, alpha: float = 1000.0) -> float:
    """Relación provisional F0 = alpha*H; no es un modelo hidrodinámico validado.

    H: altura de ola de cresta a valle [m].
    alpha: coeficiente provisional [N/m].
    """
    if H < 0:
        raise ValueError("La altura de ola H no puede ser negativa.")
    if alpha < 0:
        raise ValueError("El coeficiente alpha no puede ser negativo.")
    return alpha * H


def analizar_respuesta(
    m: float,
    c: float,
    k: float,
    w: float,
    alturas,
    alpha: float = 1000.0,
):
    """Analiza la respuesta simulada para varios escenarios de altura de ola.

    Los resultados son exploratorios mientras m_added, c, k y alpha
    no estén caracterizados/calibrados experimentalmente.

    Devuelve un ndarray con columnas [altura_de_ola_m, amplitud_maxima_m].
    """
    if m <= 0:
        raise ValueError("La masa del modelo debe ser mayor que cero.")
    if c < 0 or k < 0 or w < 0:
        raise ValueError("c, k y w deben ser no negativos.")

    resultados = []

    for H in alturas:
        F0 = altura_a_fuerza(float(H), alpha)
        t, x, v = simular(m=m, c=c, k=k, F0=F0, w=w)

        inicio_estacionario = int(len(x) * 0.7)
        x_estable = x[inicio_estacionario:]
        xmax = float(np.max(np.abs(x_estable)))
        resultados.append((float(H), xmax))

    return np.asarray(resultados, dtype=float)
