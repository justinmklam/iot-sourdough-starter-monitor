from pyathena import connect
from pyathena.pandas_cursor import PandasCursor


cursor = connect(s3_staging_dir='s3://levain-monitor-data/staging/',
                 region_name='us-west-2',
                 cursor_class=PandasCursor).cursor()

df = cursor.execute("SELECT * FROM levaindata.levaintable").as_pandas()
print(df.describe())
print(df.head())
