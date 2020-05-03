from flask import Flask, render_template, request, jsonify, make_response
import pandas as pd
import numpy as np
from random import randint
from http import HTTPStatus

app = Flask(__name__)
df_global = None

@app.route("/")
def hello():
    return render_template("index.html")


@app.route("/get_data", methods=["post"])
def get_data():
    global df_global

    start_date, start_time = request.form["start"].split()
    end_date, end_time = request.form["end"].split()
    print(start_date, start_time)

    df = pd.read_json("data.json")

    # Randomize horizontal axis
    df = df.iloc[: randint(10, df.shape[0])]
    # Randomize data
    df["date"] = df["date"].dt.strftime("%Y-%m-%d")
    df["distance"] = df["distance"] * np.random.randint(0, 100, df.shape[0])
    df["temperature"] = df["temperature"] * np.random.randint(0, 100, df.shape[0])
    df["humidity"] = df["humidity"] * np.random.randint(0, 100, df.shape[0])

    df_global = df
    data_out = {}

    for col in ["distance", "temperature", "humidity"]:
        # Output data must have column names "date" and "value" for d3 charts
        data_out[col] = (
            df[["date", col]].rename(columns={col: "value"}).to_json(orient="records")
        )

    return jsonify(data_out)


@app.route("/download_csv", methods=["GET"])
def download_csv():
    global df_global
    df = df_global

    if df is not None:
        filename = "data.csv"
        resp = make_response(df.to_csv())
        resp.headers["Content-Disposition"] = "attachment; filename=" + filename
        resp.headers["Content-Type"] = "text/csv"
    else:
        resp = ('', HTTPStatus.NO_CONTENT)
    return resp


if __name__ == "__main__":
    app.run()
