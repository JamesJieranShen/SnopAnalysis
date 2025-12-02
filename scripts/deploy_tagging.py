#!/bin/env python3
#SBATCH --job-name=snop-tag
#SBATCH --nodelist=nucomp1,nucomp2
#SBATCH --nodes=1
#SBATCH --output=/nfs/disk1/users/jierans/junk/snop-tag_deploy_%A_%a.log
#SBATCH --partition=ubuntu_short
#SBATCH --array=0-6100
#SBATCH --mem=4G
#SBATCH --cpus-per-task=1
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jierans@sas.upenn.edu

import json
import os
import subprocess
from pathlib import Path
import shutil
import numpy as np
import argparse


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--config", type=str, default="./config/bismsb/tag_run.json", help="Path to configuration file.")
    parser.add_argument("--outdir", type=str, default="/nfs/disk1/users/jierans/snoplus/bismsb_roi/tagged-files", help="Output directory for tagged files.")
    parser.add_argument("--test", action="store_true", help="Run in test mode (fake array index of 0).")
    # parser.add_argument("--workdir", type=str, default="/nfs/disk1/users/jierans/snoplus/bismsb_roi/tagged-files-working", help="Working directory for intermediate files.")
    args = parser.parse_args()

    if args.test:
        print("Running in test mode.")

    print("This is host:", os.uname().nodename)
    slurm_task_id = int(os.getenv("SLURM_ARRAY_TASK_ID")) if not args.test else 0
    task_id_list = np.loadtxt("./tasks.txt", dtype=int)
    task_id = task_id_list[slurm_task_id] if slurm_task_id is not None else None
    if task_id is not None:
        task_id = int(task_id)
    else: 
        raise ValueError()
    print("SLURM ARRAY ID:", slurm_task_id)
    print("Task ID:", task_id)


    cfg = args.config
    outdir = args.outdir
    workdir = args.outdir + "-working"
    print(f"Using config file: {args.config}")
    print(f"Output directory: {outdir}")
    print(f"Working directory: {workdir}")
    Path(outdir).mkdir(parents=True, exist_ok=True)
    Path(workdir).mkdir(parents=True, exist_ok=True)

    run_list = json.load(open("runs.json"))
    flat_list = []
    for directory, filespec in run_list.items():
        for f in filespec:
            f["directory"] = directory
            flat_list.append(f)

    run_spec = flat_list[task_id]
    print("Files to process:", run_spec)
    workfile = f"{workdir}/{run_spec['module']}_r{run_spec['run']:010d}.ntuple.root"
    outfile = f"{outdir}/{run_spec['module']}_r{run_spec['run']:010d}.ntuple.root"
    print(f"Output file: {outfile}")

    subprocess.run([
        "./bin/RunAnalysis", 
        cfg,
        "-d", run_spec['directory'],
        "-m", run_spec['module'], 
        "-r", str(run_spec["run"]),
        "-o", workfile,
    ])
    shutil.move(workfile, outfile)
