import glob
import pandas as pd
import matplotlib.dates as mdates
import matplotlib.pyplot as plt
import seaborn as sns

sns.set_theme(style="darkgrid")

files = glob.glob("*.csv")

d = []

for f in sorted(files):
    print(f)

    df = pd.read_csv(f, parse_dates=["date"]).sort_values(by=["date"], ignore_index=True)
    # df.plot(x="date", y="risepercent")
    max_index = df["risepercent"].idxmax()
    max_rise = df.loc[max_index, "risepercent"]
    start_time = df["date"].iloc[0]
    max_time = df.loc[max_index, "date"]
    duration_hours = (max_time - start_time).seconds / 3600

    # print(start_time, max_rise, duration_hours)
    d.append({
        "start_time": start_time,
        "rise_percent": max_rise,
        "duration_hours": duration_hours
    })
    # plt.show()
    # break

dates = [
    "2021-01-26",
    "2021-01-27",
    "2021-01-28",
    "2021-01-29",
    "2021-01-31",
    "2021-02-01",
    "2021-02-02",
    "2021-02-06",
    "2021-02-07",
    "2021-02-08",
    "2021-02-09",
    "2021-02-10",
    "2021-02-11",
]

for date in dates:
    d.append({
        "start_time": pd.to_datetime(date),
        "rise_percent": None,
        "duration_hours": None
    })

df_out = pd.DataFrame(d).sort_values(by=["start_time"], ignore_index=True)
df_out["start_time"] = df_out["start_time"].dt.date
df_out = df_out.set_index("start_time")

print(df_out)
colors = sns.color_palette()

fig, ax = plt.subplots(2, 1, figsize=(12, 6))
ax[0].bar(df_out.index.values, df_out["rise_percent"], color=colors[0])
ax[0].set_ylabel("Peak Height(%)")

ax[1].bar(df_out.index.values, df_out["duration_hours"], color=colors[1])
ax[1].set_ylabel("Time to Peak (Hours)")

ax[0].set_title("Peak Height Over Days")
for a in ax:
    a.xaxis.set_major_formatter(mdates.DateFormatter("%b-%d"))

plt.savefig("max-rise-over-time.png")

sns.lmplot(y="rise_percent", x="duration_hours", data=df_out)
ax = plt.gca()
ax.set_xlabel("Time to Peak (Hours)")
ax.set_ylabel("Peak Height (%)")
ax.set_title("Regression: Peak Height vs Time")
plt.tight_layout()
plt.savefig("regression.png")

plt.show()
