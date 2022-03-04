#!/usr/bin/env python3

'''
https://stackoverflow.com/questions/50714469/recursively-iterate-through-all-subdirectories-using-pathlib
'''

import argparse
import re
import shlex
import subprocess
import sys
import tqdm

from pathlib import Path
from typing import Union

import matplotlib.pyplot as plt
import pandas as pd
import plumbum


def iteration(vulture_sleep: int,
              hunter_sleep: tuple,
              hunter_interval: int,
              nimrod_timeout: int) -> None:
    '''
    Interface for running a set of nimrod
    instances using a variety of timeouts
    '''
    hsleep = range(hunter_sleep[0], hunter_sleep[1], hunter_interval)
    hsleep = hsleep.__reversed__()

    makecmd = plumbum.local['make']
    mkdircmd = plumbum.local['mkdir']['-pv']
    killcmd = plumbum.local['killall']['-q']

    makecmd('clean')
    makecmd()

    mkdircmd('./pid/')
    mkdircmd('./log/')

    for hunt_delay in tqdm.tqdm(hsleep):
        hunter_log = f'./log/hunter{hunt_delay}.log'
        vulture_log = f'./log/vulture{hunt_delay}.log'
        agent_cmd = shlex.split((
            './nimrod '
            f'-h {hunter_log} '
            f'-i {hunt_delay} '
            f'-v {vulture_log} '
            f'-w {vulture_sleep}'
        ))

        try:
            proc = subprocess.Popen(agent_cmd, start_new_session=True)
            proc.wait(timeout=nimrod_timeout)
        except subprocess.TimeoutExpired:
            timeout_msg = (
                f'Timeout for {agent_cmd} ({nimrod_timeout}s) expired\n'
                'Terminating the whole process group ...'
            )
            print(timeout_msg, file=sys.stdout)
        killcmd('vulture', retcode=(0, 1))
        killcmd('hunter', retcode=(0, 1))
        killcmd('monitor', retcode=(0, 1))


def visualize(filedir: Union[str, Path]) -> None:
    '''
    Method for visualizing the output log
    data from the vulture agent
    '''
    vulture_search = re.compile(r'(vulture)(\d*)(.log)')

    vulture_log_dir = Path(filedir)
    vulture_collection = []
    for lfile in vulture_log_dir.glob('**/*'):
        vult_res = vulture_search.match(lfile.name)
        if vult_res:
            hdelay = vult_res.group(2)
            vulture_collection.append((lfile, hdelay))

    vulture_data = {}
    for (vfile, hdelay) in tqdm.tqdm(vulture_collection):
        vdata = pd.read_csv(vfile, sep=' ', header=None)
        vdata.columns = ['HASH', 'PID']
        vulture_data[hdelay] = vdata

    (__, mainax) = plt.subplots()
    vulture_legends = []
    for (hdelay, vdata) in vulture_data.items():
        vulture_legends.append(f'{hdelay}ms')
        vdata.plot(
            y='PID',
            title='Vulture vs Hunter',
            ax=mainax,
            use_index=True,
            grid=True,
            legend=True,
            xlabel='Cycles',
            ylabel='PID'
        )
    mainax.legend(vulture_legends)
    plt.savefig(vulture_log_dir.joinpath('vultureplot.png'))


if __name__ == '__main__':
    # create the top-level parser
    parser = argparse.ArgumentParser(prog='GENESIS')
    subparsers = parser.add_subparsers(dest='base')

    iter_parser = subparsers.add_parser(name='iteration')
    iter_parser.add_argument(
        '-v', '--vsleep', type=int, dest='vsleep', default=100
    )
    iter_parser.add_argument(
        '--hsleep', nargs=2, dest='hsleep', default=[100, 1100]
    )
    iter_parser.add_argument(
        '-i', '--interval', type=int, dest='interval', default=100
    )
    iter_parser.add_argument(
        '-t', '--timeout', type=int, dest='timeout', default=300
    )
    visual_parser = subparsers.add_parser(name='visualize')
    visual_parser.add_argument(
        '-d', '--dir', type=str, dest='logdir', required=True
    )
    args = parser.parse_args()

    if args.base == 'iteration':
        hsleep = [int(sleep_val) for sleep_val in args.hsleep]
        hsleep = tuple(sorted(hsleep))
        vsleep = args.vsleep
        hsleep_step = args.interval
        nimtimeout = args.timeout
        iteration(vsleep, hsleep, hsleep_step, nimtimeout)
    elif args.base == 'visualize':
        visualize(args.logdir)
