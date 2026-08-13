import numpy as np
from scipy.integrate import solve_ivp
from src.modelo import modelo_boya

def simular(m, c, k, F0, w, t_max=60, dt=0.01):
    """
    Ejecuta la simulación temporal
    """
    t_eval = np.arange(0, t_max, dt)

    y0 = [0, 0]  # condición inicial: reposo

    sol = solve_ivp(
        modelo_boya,
        [0, t_max],
        y0,
        args=(m, c, k, F0, w),
        t_eval=t_eval,
        method='RK45'
    )

    return sol.t, sol.y[0], sol.y[1]  # tiempo, posición, velocidad