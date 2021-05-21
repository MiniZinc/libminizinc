#!/usr/bin/env python3

from pathlib import Path
from subprocess import run
from typing import List, Tuple
import argparse
import asyncio
import csv
import json
import os
import shlex
import time


async def flatten(
    instance: List[Path],
    minizinc: str,
    prefix_args: List[str],
    suffix_args: List[str],
):
    cmd = []
    cmd.extend(prefix_args)
    cmd.extend(
        [
            minizinc,
            "--solver",
            "org.minizinc.mzn-fzn",
            "-c",
            "-s",
            "-v",
        ]
    )
    cmd.extend(suffix_args)
    cmd.extend(instance)
    p = await asyncio.create_subprocess_exec(
        *cmd, stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.PIPE
    )
    stdout, stderr = await p.communicate()
    return p.returncode, stdout.decode(), stderr.decode()


async def process(
    semaphore: asyncio.Semaphore,
    count: Tuple[int, int],
    instance: List[Path],
    minizinc: str,
    prefix_args: List[str],
    suffix_args: List[str],
    root: Path,
    stats,
):
    files = [p.relative_to(root).as_posix() for p in instance]
    name = " with ".join(files)
    i, n = count
    async with semaphore:
        print(f"Flattening {name} [{i}/{n}]...")
        exit_code, stdout, stderr = await flatten(
            instance, minizinc, prefix_args, suffix_args
        )
    while len(files) < 2:
        files.append("")
    for line in stdout.splitlines():
        l = line.strip()
        if l.startswith("%%%mzn-stat: "):
            [key, val] = l[13:].split("=")
            try:
                val = json.loads(val)
            except json.JSONDecodeError:
                pass
            stats.writerow(files + [key, val])

    if exit_code == 0:
        print(f"Flattening {name} succeeded.")
        return True
    else:
        print(f"Flattening {name} failed.")
        print(stderr)
        return False


async def main(args):
    minizinc = args.minizinc
    instances = []
    version = run([minizinc, "--version"], capture_output=True, encoding="utf-8").stdout
    print("Flattening benchmarking tool\n")
    print(version)
    root = Path(args.instances)
    if root.is_dir():
        for mzn in root.rglob("*.mzn"):
            dzns = list(mzn.parent.rglob("*.dzn"))
            if len(dzns) > 0:
                for dzn in dzns:
                    instances.append([mzn, dzn])
            else:
                instances.append([mzn])
    elif root.is_file():
        with root.open("r", newline="") as fp:
            for row in csv.reader(fp):
                instances.append([Path(root.parent) / f for f in row if len(f) > 0])
        root = root.parent
    else:
        raise ValueError(f"Failed to collect instances from '{root}'")
    n = len(instances)
    max_workers = n
    if args.parallel == 0:
        max_workers = os.cpu_count()
    elif args.parallel > 0:
        max_workers = args.parallel
    semaphore = asyncio.Semaphore(max_workers)
    print(f"{n} instances collected.")
    print(f"Flattening with {max_workers} workers.\n")
    prefix_args = shlex.split(args.prefix_args)
    suffix_args = shlex.split(args.args)
    with open(args.output, "w", newline="") as csvfile:
        stats = csv.writer(csvfile)
        stats.writerow(["model", "data_file", "statistic", "value"])
        stats.writerow(["", "", "_minizincVersion", version])
        stats.writerow(["", "", "_minizincCommand", minizinc])
        stats.writerow(["", "", "_minizincPrefixArgs", prefix_args])
        stats.writerow(["", "", "_minizincSuffixArgs", suffix_args])
        results = await asyncio.gather(
            *[
                process(
                    semaphore,
                    (i, n),
                    instance,
                    minizinc,
                    prefix_args,
                    suffix_args,
                    root,
                    stats,
                )
                for i, instance in enumerate(instances)
            ]
        )
        fails = results.count(False)
        stats.writerow(["", "", "_flatFailures", fails])
        print(f"{fails}/{n} instances failed to flatten.")

    try:
        # Print summary if dependencies are available
        import pandas as pd
        from tabulate import tabulate

        df = pd.read_csv(args.output)
        df.value = pd.to_numeric(df.value, errors="coerce")
        df_s = (
            df.groupby(by=["statistic"])
            .sum(numeric_only=True)
            .reset_index(level=0)
            .sort_values(by="statistic")
        )
        print(tabulate(df_s, headers=["statistic", "total"], tablefmt="pretty"))
    except ImportError:
        pass


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run flattening benchmark.")
    parser.add_argument(
        "instances",
        type=str,
        help="instances directory",
    )
    parser.add_argument(
        "output",
        type=str,
        help="output CSV file",
    )
    parser.add_argument(
        "--minizinc",
        default="minizinc",
        type=str,
        help="Path to the minizinc binary to use",
    )
    parser.add_argument(
        "--prefix-args",
        default="",
        type=str,
        help="Command line options to add to beginning of minizinc call",
    )
    parser.add_argument(
        "--args",
        default="",
        type=str,
        help="Command line options to add to end of minizinc call",
    )
    parser.add_argument(
        "--parallel",
        default=0,
        type=int,
        help="Parallel instances to flatten",
    )

    start = time.time()
    asyncio.run(main(parser.parse_args()))
    print(f"Done in {time.time() - start}s.")
