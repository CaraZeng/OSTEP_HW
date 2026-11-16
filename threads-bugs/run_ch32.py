import subprocess
import re
import csv
import statistics

programs = [
    "vector-global-order",
    "vector-try-wait",
    "vector-avoid-hold-and-wait",
    "vector-nolock",
]

# Q5: Different thread counts and loop counts
# Q6: Parallel vs shared vectors
# Q7-Q10: Compare implementations
tests = []

for program in programs:
    for parallel in [False, True]:
        for threads in [2, 4, 8]:
            for loops in [1000, 10000, 100000]:
                tests.append((program, parallel, threads, loops))

results = []

with open("results/ch32_experiments.txt", "w") as log:
    for program, parallel, threads, loops in tests:

        cmd = [
            "./" + program,
            "-n", str(threads),
            "-l", str(loops),
            "-t",
        ]

        if parallel:
            cmd.append("-p")

        times = []
        retries = []
        statuses = []

        for run in range(3):
            try:
                result = subprocess.run(
                    cmd,
                    capture_output=True,
                    text=True,
                    timeout=30
                )

                if result.returncode != 0:
                    statuses.append(f"EXIT_{result.returncode}")
                    continue

                match = re.search(
                    r"Time:\s+([\d.]+)",
                    result.stdout
                )

                if match:
                    times.append(float(match.group(1)))

                retry_match = re.search(
                    r"Retries:\s+(\d+)",
                    result.stdout
                )

                if retry_match:
                    retries.append(int(retry_match.group(1)))

                statuses.append("OK")

            except subprocess.TimeoutExpired:
                statuses.append("TIMEOUT")

        avg_time = (
            statistics.mean(times)
            if times else None
        )

        avg_retries = (
            statistics.mean(retries)
            if retries else None
        )

        row = {
            "program": program,
            "parallel": parallel,
            "threads": threads,
            "loops": loops,
            "avg_time": avg_time,
            "avg_retries": avg_retries,
            "status": ",".join(statuses),
        }

        results.append(row)

        line = (
            f"{program:28} "
            f"parallel={str(parallel):5} "
            f"threads={threads:2} "
            f"loops={loops:6} "
            f"time={str(avg_time):8} "
            f"retries={str(avg_retries):8} "
            f"status={row['status']}"
        )

        print(line)
        log.write(line + "\n")

with open("results/ch32_results.csv", "w", newline="") as f:
    writer = csv.DictWriter(
        f,
        fieldnames=results[0].keys()
    )

    writer.writeheader()
    writer.writerows(results)

print("\nExperiments completed.")
print("Saved results/ch32_results.csv")
