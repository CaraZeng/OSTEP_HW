import subprocess
import re
import matplotlib.pyplot as plt

# Job lengths to test.
# Log-spaced values make the graph similar to Figure 9.2.
job_lengths = [1, 2, 5, 10, 20, 50, 100, 200, 500, 1000]

# The textbook averages each point over 30 trials.
trials = 30

average_fairness = []

for runtime in job_lengths:
    fairness_values = []

    for seed in range(1, trials + 1):

        command = [
            "python3",
            "lottery.py",
            "-s", str(seed),
            "-l", f"{runtime}:100,{runtime}:100",
            "-c"
        ]

        result = subprocess.run(
            command,
            capture_output=True,
            text=True
        )

        # Find completion times from lines such as:
        # --> JOB 0 DONE at time 196
        completion_times = re.findall(
            r"DONE at time (\d+)",
            result.stdout
        )

        completion_times = [
            int(time) for time in completion_times
        ]

        if len(completion_times) == 2:
            first = completion_times[0]
            second = completion_times[1]

            fairness = first / second
            fairness_values.append(fairness)

    average = sum(fairness_values) / len(fairness_values)
    average_fairness.append(average)

    print(
        f"R={runtime:4d}, "
        f"Average Fairness={average:.4f}"
    )


plt.figure(figsize=(8, 5))

plt.plot(
    job_lengths,
    average_fairness,
    marker="o"
)

plt.xscale("log")

plt.xlabel("Job Length")
plt.ylabel("Fairness")
plt.title("Lottery Scheduling Fairness")

plt.ylim(0, 1.05)
plt.grid(True)

plt.savefig(
    "lottery_fairness.png",
    dpi=300,
    bbox_inches="tight"
)

plt.show()