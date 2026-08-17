import numpy as np
from scipy.integrate import solve_ivp

from backend.app.models.buoy import modelo_boya


def simular(
    m,
    c,
    k,
    F0,
    w,
    t_max=60.0,
    dt=0.01
):
    """
    Ejecuta una simulación temporal del modelo de la boya.

    Returns:
        t -> vector de tiempo
        x -> desplazamiento
        v -> velocidad
    """

    if m <= 0:
        raise ValueError("La masa debe ser mayor que cero.")

    if dt <= 0:
        raise ValueError("dt debe ser mayor que cero.")

    if t_max <= 0:
        raise ValueError("t_max debe ser mayor que cero.")

    t_eval = np.arange(0.0, t_max, dt)

    y0 = [0.0, 0.0]

    sol = solve_ivp(
        modelo_boya,
        [0.0, t_max],
        y0,
        args=(m, c, k, F0, w),
        t_eval=t_eval,
        method="RK45"
    )

    if not sol.success:
        raise RuntimeError(
            f"La simulación falló: {sol.message}"
        )

    return sol.t, sol.y[0], sol.y[1]