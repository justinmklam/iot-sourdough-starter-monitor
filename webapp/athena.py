from pyathena import connect
from pyathena.pandas.util import as_pandas


connection = connect(s3_staging_dir='s3://levain-monitor-data/staging/',
                 region_name='us-west-2')

cursor1 = connection.cursor()

query = """
select sessionid, min(time) as startTime, max(time) as endTime, date_diff('minute', min(time), max(time)) as durationMin
from levaindatabase.levain_table group by sessionid
order by startTime desc
"""

cursor1.execute(query)

df1 = as_pandas(cursor1)
print(df1.head())


session_id = 1782426941
cursor2 = connection.cursor()
query = f"select * from levaindatabase.levain_table where sessionid = {session_id}"

cursor2.execute(query)
df2 = as_pandas(cursor2)
print(df2.head())

