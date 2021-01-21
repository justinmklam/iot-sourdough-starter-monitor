from pyathena import connect
from pyathena.pandas.util import as_pandas

from dotenv import load_dotenv
load_dotenv()


class Database:
    S3_BUCKET = 's3://levain-monitor-data/staging/'
    S3_REGION = 'us-west-2'
    TABLE_NAME = "levaindatabase.levain_table"

    def __init__(self):
        self.connection = connect(s3_staging_dir=self.S3_BUCKET, region_name=self.S3_REGION)

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

        return as_pandas(cursor)

    def get_data_from_session(self, session_id: id):
        cursor = self.connection.cursor()
        query = f"select * from {self.TABLE_NAME} where sessionid = {session_id}"

        cursor.execute(query)
        return as_pandas(cursor)


if __name__ == "__main__":
    db = Database()

    df_sessions = db.get_sessions()
    print(df_sessions)

    session_id = df_sessions["sessionid"][0]
    print("Session ID: ", session_id)

    df_session_data = db.get_data_from_session(session_id)
    print(df_session_data.head())

