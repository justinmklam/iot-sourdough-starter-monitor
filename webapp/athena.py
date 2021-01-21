from pyathena import connect
from pyathena.pandas.util import as_pandas


cursor = connect(s3_staging_dir='s3://levain-monitor-data/staging/',
                 region_name='us-west-2').cursor()

query = """
select sessionid, min(time) as startTime, max(time) as endTime, date_diff('minute', min(time), max(time)) as durationMin
from levaindatabase.levain_table group by sessionid
order by startTime desc
"""

cursor.execute(query)

df = as_pandas(cursor)
print(df.describe())
print(df.head())
