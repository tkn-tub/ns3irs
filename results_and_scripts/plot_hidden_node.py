import pandas as pd
import matplotlib.pyplot as plt

# plotting config
columnwidth = 4.5  # TODO set to 1 column layout perhaps 9?
textwidth = columnwidth * 2 + 0.25
figsize = (columnwidth, columnwidth * 0.6)

fontsize_big = 10
fontsize_small = 8
params = {
    # Avoid type 3 fonts
    "pdf.fonttype": 42,
    "font.family": "serif",
    "font.serif": "Times New Roman",
    "font.sans-serif": [],
    "font.monospace": [],
    # Make the legend/label fonts a little smaller
    "font.size": fontsize_big,
    "axes.labelsize": fontsize_big,
    "axes.titlesize": fontsize_big,
    "legend.fontsize": fontsize_small,
    "legend.title_fontsize": fontsize_small,
    "xtick.labelsize": fontsize_small,
    "ytick.labelsize": fontsize_small,
    "figure.figsize": figsize,
    "figure.autolayout": True,
    # save some space around figures when saving
    "savefig.bbox": "tight",
    "savefig.pad_inches": 0.025,
}
pdf_params = {
    "text.usetex": True,
    "pgf.texsystem": "pdflatex",
    "pgf.rcfonts": False,
    "pgf.preamble": "\n".join(
        [
            # put LaTeX preamble declarations here
            r"\usepackage[utf8x]{inputenc}",
            r"\usepackage[T1]{fontenc}",
        ]
    ),
}

plt.rcParams['patch.linewidth'] = 0
plt.rcParams['patch.edgecolor'] = 'none'

# for PFG plotting:
# params.update(**pdf_params)
plt.rcParams.update(params)

run_numbers = range(1, 50)
# run_numbers = range(1, 1)

dfs = []
for run in run_numbers:
    df = pd.read_csv(f"hidden-node-problem/hidden-node-problem_{run}.csv")
    df["Run"] = run  # Track which run it came from
    dfs.append(df)
df_all = pd.concat(dfs)
# df_all = pd.read_csv(f"hidden-node/hidden-node-problem_1.csv")
df_avg = df_all.groupby(["Time", "Scenario"], as_index=False)[["Tx1_Throughput", "Tx2_Throughput"]].mean()

colors = {
    "Baseline": "#017f3f",  # rgb(1, 127, 63)
    "RIS": "#0d8ad8",  # rgb(13, 138, 216)
    "RTS/CTS": "#903bab"  # rgb(144, 59, 171)
}

plt.figure(figsize=(10, 6))
for scenario in df_avg["Scenario"].unique():
    subset = df_avg[df_avg["Scenario"] == scenario]
    if scenario == "IRS":
        scenario = "RIS"
    plt.plot(subset["Time"], subset["Tx1_Throughput"], label=f"Node A - {scenario}", color=colors.get(scenario, "black"))
    plt.plot(subset["Time"], subset["Tx2_Throughput"], label=f"Node C - {scenario}", linestyle="dashed", color=colors.get(scenario, "black"))

plt.xlabel("Time (s)")
plt.ylabel("Throughput (Mbps)")
plt.xlim(1, 15)
plt.legend()
plt.grid()

plt.savefig("hidden_node_problem.pdf")
plt.show()
