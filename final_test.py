
import subprocess
import re

cases = [
    ("2 coders", 2, 1000, 200, 200, 100, 3, 50),
    ("3 coders", 3, 1000, 200, 200, 100, 3, 50),
    ("5 coders", 5, 1000, 200, 200, 100, 3, 50),
    ("7 coders", 7, 1500, 200, 200, 100, 3, 50),
    ("10 coders", 10, 2000, 200, 200, 100, 3, 50),
    ("zero cooldown", 5, 1000, 200, 200, 100, 3, 0),
]

pattern = re.compile(
    r"^(\d+) (\d+) "
    r"(has taken a dongle|is compiling|is debugging|"
    r"is refactoring|burned out)$"
)

passed = 0
failed = 0
total = len(cases) * 2 * 5
current = 0

for name, n, burnout, compile_ms, debug, refactor, required, cooldown in cases:
    for scheduler in ("fifo", "edf"):
        for run in range(1, 6):
            current += 1

            args = [
                "./codexion",
                str(n),
                str(burnout),
                str(compile_ms),
                str(debug),
                str(refactor),
                str(required),
                str(cooldown),
                scheduler,
            ]

            ok = False
            detail = ""

            try:
                result = subprocess.run(
                    args,
                    capture_output=True,
                    text=True,
                    timeout=20,
                )

                lines = result.stdout.splitlines()
                compiles = {i: 0 for i in range(1, n + 1)}
                dongles = {i: 0 for i in range(1, n + 1)}
                deaths = 0
                valid_logs = True
                previous_time = -1

                for line in lines:
                    match = pattern.fullmatch(line)

                    if match is None:
                        valid_logs = False
                        continue

                    timestamp = int(match.group(1))
                    coder = int(match.group(2))
                    action = match.group(3)

                    if coder < 1 or coder > n:
                        valid_logs = False
                        continue

                    if timestamp < previous_time:
                        valid_logs = False

                    previous_time = timestamp

                    if action == "is compiling":
                        compiles[coder] += 1
                    elif action == "has taken a dongle":
                        dongles[coder] += 1
                    elif action == "burned out":
                        deaths += 1

                correct_compiles = all(
                    compiles[i] == required
                    for i in range(1, n + 1)
                )

                correct_dongles = all(
                    dongles[i] == required * 2
                    for i in range(1, n + 1)
                )

                ok = (
                    result.returncode == 0
                    and len(lines) > 0
                    and valid_logs
                    and correct_compiles
                    and correct_dongles
                    and deaths == 0
                )

                detail = (
                    f"compiles={sum(compiles.values())}/{n * required}, "
                    f"dongles={sum(dongles.values())}/{n * required * 2}, "
                    f"burnouts={deaths}"
                )

                if not valid_logs:
                    detail += ", invalid log format/order"

                if result.returncode != 0:
                    detail += f", exit={result.returncode}"

                if result.stderr:
                    detail += f", stderr={result.stderr.strip()[:200]}"

            except subprocess.TimeoutExpired:
                detail = "TIMEOUT (over 20 seconds)"

            if ok:
                passed += 1
                print(
                    f"[{current}/{total}] PASS: "
                    f"{name} {scheduler} run {run}",
                    flush=True,
                )
            else:
                failed += 1
                print(
                    f"[{current}/{total}] FAIL: "
                    f"{name} {scheduler} run {run} ({detail})",
                    flush=True,
                )

print("\n----------------------")
print(f"TOTAL:  {total}")
print(f"PASSED: {passed}")
print(f"FAILED: {failed}")

if failed == 0:
    print("All functional stress tests passed!")
else:
    print("Some tests failed. Review the results above.")

