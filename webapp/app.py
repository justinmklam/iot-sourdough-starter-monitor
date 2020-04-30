from flask import Flask, render_template
import pandas as pd

app = Flask(__name__)


@app.route('/')
def hello():
    return render_template("index.html")


@app.route("/get_data", methods=["GET"])
def get_data():
    df = pd.DataFrame({
        "date": ["2020-01-01", "2020-01-02"],
        "temperature": [1, 2],
        "humidity": [3, 4],
        "distance": [5, 6]
    })

    return df.to_json(orient="records")


if __name__ == '__main__':
    app.run()
