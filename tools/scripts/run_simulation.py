from __future__ import annotations

import json
import sys
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

PROJECT_ROOT = Path(__file__).resolve().parents[2]
if str(PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(PROJECT_ROOT))

from backend.app.services.analysis_service import analizar_respuesta


def load_json(relative_path: str) -> dict:
    path = PROJECT_ROOT / relative_path
    if not path.is_file():
        raise FileNotFoundError(f"No se encontró el archivo de parámetros: {path}")
    with path.open("r", encoding="utf-8") as file:
        return json.load(file)


def main() -> None:
    physical = load_json("config/physical_parameters.json")
    mechanical = load_json("config/mechanical_parameters.json")
    experiment = load_json("config/experiment_defaults.json")

    # Primera aproximación numérica: masa seca medida del conjunto móvil.
    # La masa añadida no está determinada y no se incorpora en esta corrida.
    m_model = float(physical["buoy"]["moving_assembly_dry_mass_kg"])

    # Parámetros provisionales, no calibrados.
    c = float(mechanical["buoy_model"]["damping_N_s_per_m"])
    k = float(mechanical["buoy_model"]["stiffness_N_per_m"])
    alpha = float(mechanical["buoy_model"]["force_height_coefficient_N_per_m"])

    # Velocidad de referencia medida a PWM 1600 us; no es PWM 2000 us.
    w = float(experiment["measured_servo_reference"]["angular_velocity_rad_s"])

    # Alturas teóricas derivadas de 25-45% de la carrera geométrica ideal.
    # No son mediciones del HC-SR04.
    alturas = np.asarray(experiment["wave_height_scenarios_m"], dtype=float)

    resultados = analizar_respuesta(
        m=m_model,
        c=c,
        k=k,
        w=w,
        alturas=alturas,
        alpha=alpha,
    )

    output_data = PROJECT_ROOT / "data" / "processed"
    output_data.mkdir(parents=True, exist_ok=True)
    np.savetxt(
        output_data / "altura_vs_desplazamiento.csv",
        resultados,
        delimiter=",",
        header="Altura_ola_m,Amplitud_maxima_boya_m",
        comments="",
    )

    output_figures = PROJECT_ROOT / "results" / "figures"
    output_figures.mkdir(parents=True, exist_ok=True)

    plt.figure(figsize=(8, 5))
    plt.plot(resultados[:, 0], resultados[:, 1], marker="o", linewidth=1.5)
    plt.xlabel("Altura de ola H (m) - escenario teórico")
    plt.ylabel("Amplitud máxima simulada (m)")
    plt.title("MARVOLT-PA - Sensibilidad preliminar a H")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_figures / "altura_vs_desplazamiento.png", dpi=300)
    plt.show()

    print("\nCorrida exploratoria completada.")
    print(f"Masa seca introducida: {m_model:.5f} kg")
    print(f"Velocidad de referencia: {w:.3f} rad/s (PWM 1600 us)")
    print(f"Coeficientes c={c:g}, k={k:g}, alpha={alpha:g} permanecen provisionales.")
    print("La corrida no representa la prueba actual a PWM 2000 us ni una calibración final.\n")


if __name__ == "__main__":
    main()
