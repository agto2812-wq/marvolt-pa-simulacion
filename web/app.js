fetch("../data/resultados.csv")
    .then(response => response.text())
    .then(data => {
        const filas = data.split("\n").slice(1);

        let H = [];
        let X = [];

        filas.forEach(fila => {
            if (fila.trim() === "") return;

            let [h, x] = fila.split(",");
            H.push(parseFloat(h));
            X.push(parseFloat(x));
        });

        new Chart(document.getElementById("grafica"), {
            type: "line",
            data: {
                labels: H,
                datasets: [{
                    label: "Desplazamiento",
                    data: X
                }]
            }
        });
    });