#!/usr/bin/env python3

import argparse
import plumbum
import shlex
import subprocess
import sys
import tqdm


def iteration(vulture_sleep: int,
              hunter_sleep: tuple,
              hunter_interval: int,
              nimrod_timeout: int) -> None:
    """
    Interface for running a set of nimrod
    instances using a variety of timeouts
    """
    hsleep = range(hunter_sleep[0], hunter_sleep[1], hunter_interval)
    hsleep = hsleep.__reversed__()

    makecmd = plumbum.local["make"]
    mkdircmd = plumbum.local["mkdir"]["-pv"]
    killcmd = plumbum.local['killall']['-q']

    makecmd("clean")
    makecmd()

    mkdircmd("./pid/")
    mkdircmd("./log/")

    for ht in tqdm.tqdm(hsleep):
        hunter_log = f'./log/hunter{ht}.log'
        vulture_log = f'./log/vulture{ht}.log'
        agent_cmd = shlex.split((
            './nimrod '
            f'-h {hunter_log} '
            f'-i {ht} '
            f'-v {vulture_log} '
            f'-w {vulture_sleep}'
        ))

        try:
            p = subprocess.Popen(agent_cmd, start_new_session=True)
            p.wait(timeout=nimrod_timeout)
        except subprocess.TimeoutExpired:
            timeout_msg = (
                f"Timeout for {agent_cmd} ({nimrod_timeout}s) expired\n"
                "Terminating the whole process group ..."
            )
            print(timeout_msg, file=sys.stdout)
        killcmd('vulture', retcode=(0, 1))
        killcmd('hunter', retcode=(0, 1))
        killcmd('monitor', retcode=(0, 1))


if __name__ == "__main__":
    # create the top-level parser
    parser = argparse.ArgumentParser(prog='GENESIS')

    parser.add_argument(
        '-v', '--vsleep', type=int, dest='vsleep', default=100
    )
    parser.add_argument(
        '--hsleep', nargs=2, dest='hsleep', default=[100, 1100]
    )
    parser.add_argument(
        '-i', '--interval', type=int, dest='interval', default=100
    )
    parser.add_argument(
        '-t', '--timeout', type=int, dest='timeout', default=300
    )
    args = parser.parse_args()

    vsleep = args.vsleep
    hsleep = tuple(sorted(args.hsleep))
    hsleep_step = args.interval
    nimtimeout = args.timeout
    iteration(vsleep, hsleep, hsleep_step, nimtimeout)
