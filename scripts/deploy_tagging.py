#!/bin/env python3
#SBATCH --job-name=snop-tag
#SBATCH --output=/nfs/disk1/users/jierans/junk/snop-tag_deploy_%A_%a.log
#SBATCH --partition=ubuntu_long
#SBATCH --array=0-733
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jierans@sas.upenn.edu

import json
import os
import subprocess
from pathlib import Path
print(os.getcwd())

task_id = os.getenv("SLURM_ARRAY_TASK_ID")
if task_id is not None:
    task_id = int(task_id)
else: 
    raise ValueError()
outdir = "/nfs/disk1/users/jierans/snoplus/bismsb_roi/tagged-files"
Path(outdir).mkdir(parents=True, exist_ok=True)

run_list = json.load(open("runs.json"))
flat_list = []
for directory, filespec in run_list.items():
    for f in filespec:
        f["directory"] = directory
        flat_list.append(f)

run_spec = flat_list[task_id]
print("This is host:", os.uname().nodename)
print("Task ID:", task_id)
print("Files to process:", run_spec)
outfile = f"{outdir}/{run_spec['module']}_r{run_spec['run']:010d}.ntuple.root"
print(f"Output file: {outfile}")

subprocess.run([
    "./bin/RunAnalysis", 
    "./config/tag_run.json",
    "-d", run_spec['directory'],
    "-m", run_spec['module'], 
    "-r", str(run_spec["run"]),
    "-o", outfile,
])
