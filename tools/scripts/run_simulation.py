from pathlib import Path
import sys

import matplotlib.pyplot as plt
import numpy as np


# ============================================================
# RUTA RAÍZ DEL PROYECTO
# ============================================================

PROJECT_ROOT = Path(__file__).resolve().parents[2]

if str(PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(PROJECT_ROOT))


from backend.app.services.analysis_service import analizar_respuesta


def main():
    # ============================================================
    # PARÁMETROS PROVISIONALES
    # ============================================================
    #
    # Estos parámetros pertenecen al modelo preliminar
    # masa-resorte-amortiguador.
    #
    # En la Fase 2 se trasladarán a una única fuente de verdad.
    #

    m = 100.0       # kg
    c = 50.0        # N·s/m
    k = 2000.0      # N/m
    w = 2.0         # rad/s

    # Rango provisional de alturas de ola
    alturas = np.linspace(0.5, 2.0, 10)

    # ============================================================
    # EJECUTAR ANÁLISIS
    # ============================================================

    resultados = analizar_respuesta(
        m=m,
        c=c,
        k=k,
        w=w,
        alturas=alturas
    )

    H = resultados[:, 0]
    X = resultados[:, 1]

    # ============================================================
    # GUARDAR RESULTADOS PROCESADOS
    # ============================================================

    output_data = PROJECT_ROOT / "data" / "processed"
    output_data.mkdir(parents=True, exist_ok=True)

    np.savetxt(
        output_data / "altura_vs_desplazamiento.csv",
        resultados,
        delimiter=",",
        header="Altura,Desplazamiento",
        comments=""
    )

    # ============================================================
    # GENERAR FIGURA
    # ============================================================

    output_figures = PROJECT_ROOT / "results" / "figures"
    output_figures.mkdir(parents=True, exist_ok=True)

    plt.figure(figsize=(8, 5))

    plt.plot(
        H,
        X,
        marker="o",
        linewidth=1.5
    )

    plt.xlabel("Altura de ola (m)")
    plt.ylabel("Desplazamiento máximo (m)")
    plt.title("Altura de ola vs desplazamiento de la boya")
    plt.grid(True)

    plt.tight_layout()

    plt.savefig(
        output_figures / "altura_vs_desplazamiento.png",
        dpi=300
    )

    plt.show()


if __name__ == "__main__":
    main()