from pyathena import connect
from pyathena.pandas.util import as_pandas

S3_BUCKET = 's3://levain-monitor-data/staging/'
S3_REGION = 'us-west-2'
TABLE_NAME = "levaindatabase.levain_table"

connection = connect(s3_staging_dir=S3_BUCKET, region_name=S3_REGION)

cursor1 = connection.cursor()

query = f"""
select sessionid,
min(time) as startTime,
max(time) as endTime,
date_diff('minute', min(time), max(time)) as durationMin
from {TABLE_NAME} group by sessionid
order by startTime desc
"""

cursor1.execute(query)

df1 = as_pandas(cursor1)
print(df1.head())


session_id = 1782426941
cursor2 = connection.cursor()
query = f"select * from {TABLE_NAME} where sessionid = {session_id}"

cursor2.execute(query)
df2 = as_pandas(cursor2)
print(df2.head())

