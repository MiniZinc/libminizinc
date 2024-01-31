#!/usr/bin/env python3

import argparse
import math
import numpy as np
import pandas as pd
import sys
import html

from tabulate import tabulate


def stat_change(row):
    # Create [difference, percent_change, modified_percent_change]
    if row.value_old == row.value_new:
        return pd.Series([0, 0, 0])
    try:
        old = float(row.value_old)
        new = float(row.value_new)
    except ValueError:
        return pd.Series([np.NaN, np.NaN, np.NaN])

    if old == 0:
        return pd.Series([new, np.NaN, np.NaN])

    diff = new - old
    change = diff / old
    modified = change

    if row.statistic == "flatTime":
        # Reduce importance of tiny times
        t_old = math.ceil(old * 2)
        t_new = math.ceil(new * 2)
        t_change = (t_new - t_old) / t_old
        if abs(t_change) < abs(change):
            modified = t_change

    return pd.Series([diff, change, modified])


def summary_delta(row):
    # Create [difference, percentage_change]
    if row.total_old == row.total_new:
        return pd.Series([0, 0])
    if row.total_old == 0:
        return pd.Series([row.total_new, np.NaN])

    return pd.Series(
        [row.total_new - row.total_old, (row.total_new - row.total_old) / row.total_old]
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Compare flattening benchmark runs.")
    parser.add_argument(
        "baseline",
        type=str,
        help="baseline statistics CSV file",
    )
    parser.add_argument(
        "observed",
        type=str,
        help="new statistics CSV file",
    )
    parser.add_argument(
        "output",
        type=str,
        help="output HTML file",
    )
    parser.add_argument(
        "--threshold",
        default=0.1,
        type=float,
        help="threshold for showing changes",
    )

    args = parser.parse_args()

    df1 = pd.read_csv(args.baseline)
    df2 = pd.read_csv(args.observed)
    t = args.threshold

    df = pd.merge(
        df1, df2, on=["model", "data_file", "statistic"], suffixes=["_old", "_new"]
    )

    # Per instance changed statistics
    df[["difference", "change", "modified"]] = df.apply(stat_change, axis=1)
    df_c = (
        df.loc[
            (df.model == "")
            | df.modified.isna()
            | (df.modified >= t)
            | (df.modified <= -t)
        ]
        .drop(["modified"], axis=1)
        .sort_values(by=["model", "data_file", "statistic"])
    )

    # Summary statistics
    df["total_old"] = pd.to_numeric(df.value_old, errors="coerce")
    df["total_new"] = pd.to_numeric(df.value_new, errors="coerce")
    df_s = (
        df.drop(["difference", "change", "modified"], axis=1)
        .groupby(by=["statistic"])
        .sum(numeric_only=True)
        .reset_index(level=0)
        .sort_values(by="statistic")
    )
    df_s[["difference", "change"]] = df_s.apply(summary_delta, axis=1)

    print(tabulate(df_s, headers="keys", tablefmt="pretty"))

    with open(args.output, "w", encoding="utf-8") as fp:
        summary = df_s.to_html(
            index=False,
            formatters={
                "total_old": "{:,}".format,
                "total_new": "{:,}".format,
                "difference": "{:,}".format,
                "change": "{:+,.2%}".format,
            },
        )
        details = df_c.to_html(
            index=False,
            formatters={
                # "value_old": "{:,}".format,
                # "value_new": "{:,}".format,
                "difference": "{:,}".format,
                "change": "{:+,.2%}".format,
            },
        )
        fp.write(
            f"""<!DOCTYPE html>
            <html>
                <head>
                    <meta charset="utf-8">
                    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/jquery.tablesorter/2.31.3/css/theme.blue.min.css"/>
                    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.6.0/jquery.slim.js"></script>
                    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery.tablesorter/2.31.3/js/jquery.tablesorter.min.js"></script>
                    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery.tablesorter/2.31.3/js/jquery.tablesorter.widgets.min.js"></script>
                    <style>
                        @import url('https://fonts.googleapis.com/css2?family=Open+Sans&display=swap');
                        html, body {{
                            font-family: 'Open Sans', sans-serif;
                        }}
                    </style>
                </head>
                <body>
                    <h2>Flattening benchmarks</h2>
                    <p>Generated with <pre><code>{html.escape(" ".join(sys.argv))}</code></pre></p>
                    <h3>Summary</h3>
                    {summary}
                    <details>
                        <summary>Per-instance significant changes</summary>
                        {details}
                    </details>
                    <script>
                        $("table").tablesorter({{
                            theme: 'blue',
                            widgets: ["zebra", "filter"]
                        }});
                    </script>
                </body>
            </html>    
            """
        )
