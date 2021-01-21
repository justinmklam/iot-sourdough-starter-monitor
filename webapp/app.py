from flask import Flask, render_template, request, jsonify, make_response
import pandas as pd
import numpy as np
from random import randint
from http import HTTPStatus

from database import Database

app = Flask(__name__)
db = Database()
df_global = None


@app.route("/")
def hello():
    sessions = db.get_sessions()
    print(sessions)
    return render_template(
        "index.html",
        sessions=zip(
            sessions["sessionid"], sessions["startTime"], sessions["durationMin"]
        ),
    )


@app.route("/get_data", methods=["post"])
def get_data():
    global df_global

    session_id = request.form["sessionId"]
    print(session_id)

    df = db.get_data_from_session(session_id)
    df["date"] = df["time"].dt.strftime("%Y-%m-%d %H:%M:%S")
    print(df)

    df_global = df
    data_out = {}

    for col in ["risepercent", "temperature", "humidity"]:
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
        resp = ("", HTTPStatus.NO_CONTENT)
    return resp


if __name__ == "__main__":
    app.run()
