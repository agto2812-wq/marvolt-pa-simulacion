import numpy as np
import matplotlib.pyplot as plt
import os

from src.analisis import analizar_respuesta

# Parámetros del sistema
m = 100       # kg
c = 50        # Ns/m
k = 2000      # N/m
w = 2         # rad/s

# Rango de alturas de ola
alturas = np.linspace(0.5, 2.0, 10)

# Ejecutar análisis
resultados = analizar_respuesta(m, c, k, w, alturas)

# Separar datos
H = resultados[:, 0]
X = resultados[:, 1]

# Guardar CSV
os.makedirs("data", exist_ok=True)
np.savetxt("data/resultados.csv", resultados, delimiter=",", header="Altura,Desplazamiento", comments='')

# Graficar
plt.figure()
plt.plot(H, X, marker='o')
plt.xlabel("Altura de ola (m)")
plt.ylabel("Desplazamiento máximo (m)")
plt.title("Altura de ola vs desplazamiento de la boya")
plt.grid()

os.makedirs("plots", exist_ok=True)
plt.savefig("plots/grafica.png")
plt.show()