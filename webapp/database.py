import os
import pandas as pd
from pyathena import connect
from pyathena.pandas.util import as_pandas

from dotenv import load_dotenv
load_dotenv()


class Database:
    S3_BUCKET = "s3://levain-monitor-data/staging/"
    S3_REGION = "us-west-2"
    TABLE_NAME = "levaindatabase.levain_table"
    UTC_OFFSET = -8  # to PDT

    def __init__(self):
        self.connection = connect(
            s3_staging_dir=self.S3_BUCKET,
            region_name=self.S3_REGION,
            aws_access_key_id=os.environ["AWS_ACCESS_KEY_ID"],
            aws_secret_access_key=os.environ["AWS_SECRET_ACCESS_KEY"]
        )

    def get_sessions(self):
        cursor = self.connection.cursor()

        query = f"""
        select sessionid,
        min(time) as startTime,
        max(time) as endTime,
        date_diff('minute', min(time), max(time)) as durationMin
        from {self.TABLE_NAME} group by sessionid
        order by startTime desc
        """

        cursor.execute(query)

        df = as_pandas(cursor)
        df["startTime"] = df["startTime"] + pd.Timedelta(hours=self.UTC_OFFSET)
        df["endTime"] = df["endTime"] + pd.Timedelta(hours=self.UTC_OFFSET)
        return df

    def get_data_from_session(self, session_id: id):
        cursor = self.connection.cursor()
        query = f"select * from {self.TABLE_NAME} where sessionid = {session_id}"

        cursor.execute(query)

        df = as_pandas(cursor)
        df["time"] = df["time"] + pd.Timedelta(hours=self.UTC_OFFSET)
        return df


if __name__ == "__main__":
    db = Database()

    df_sessions = db.get_sessions()
    print(df_sessions)

    session_id = df_sessions["sessionid"][0]
    print("Session ID: ", session_id)

    df_session_data = db.get_data_from_session(session_id)
    print(df_session_data.head())
