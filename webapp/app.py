from flask import Flask, render_template, request, jsonify, make_response, session
import pandas as pd
from http import HTTPStatus

from database import Database

app = Flask(__name__)
app.secret_key = "dsaklj^$##&jdjsaidjsiao&&$"
db = Database()

sessions = db.get_sessions()
print(sessions)


@app.route("/")
def hello():
    # Will use cached response if available
    sessions = db.get_sessions()
    return render_template(
        "index.html",
        sessions=zip(
            sessions["sessionid"],
            sessions["startTime"].dt.strftime("%B %d, %Y, %r"),
            pd.to_datetime(sessions["durationMin"], unit="m").dt.strftime("%-H hours, %-M min")
        ),
    )


@app.route("/get_data", methods=["post"])
def get_data():
    session_id = request.form["sessionId"]
    print(session_id)

    df = db.get_data_from_session(session_id)
    df["date"] = df["time"].dt.strftime("%Y-%m-%d %H:%M:%S")
    df.drop(columns=["time"])
    print(df)

    session["df"] = df.to_dict()
    data_out = {}

    for col in ["risepercent", "temperature", "humidity"]:
        # Output data must have column names "date" and "value" for d3 charts
        data_out[col] = (
            df[["date", col]].rename(columns={col: "value"}).to_json(orient="records")
        )

    return jsonify(data_out)


@app.route("/download_csv", methods=["GET"])
def download_csv():
    dict_obj = session.get("df", None)
    df = pd.DataFrame(dict_obj)

    if df is not None:
        start_time = df["time"].dt.strftime("%Y%m%d-%H%M%S").iloc[0]
        end_time = df["time"].dt.strftime("%Y%m%d-%H%M%S").iloc[-1]
        session_id = df["sessionid"].iloc[0]

        filename = f"{session_id}_{start_time}_{end_time}.csv"
        resp = make_response(df.to_csv())
        resp.headers["Content-Disposition"] = "attachment; filename=" + filename
        resp.headers["Content-Type"] = "text/csv"
    else:
        resp = ("", HTTPStatus.NO_CONTENT)
    return resp


if __name__ == "__main__":
    app.run()
