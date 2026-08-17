from fastapi import FastAPI

app = FastAPI(
    title="MARVOLT-PA API",
    version="0.1.0"
)

@app.get("/")
def root():
    return {
        "project": "MARVOLT-PA",
        "status": "online"
    }