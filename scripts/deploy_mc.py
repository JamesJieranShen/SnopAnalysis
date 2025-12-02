#!/bin/env python3
#SBATCH --job-name=snop-tag
##SBATCH --nodelist=nucomp1,nucomp2
#SBATCH --nodes=1
#SBATCH --output=/nfs/disk1/users/jierans/junk/snop-mc-tag_%A_%a.log
#SBATCH --partition=gpu
#SBATCH --array=0-64
#SBATCH --mem=8G
#SBATCH --mail-type=ALL
#SBATCH --mail-user=jierans@sas.upenn.edu

import json5 as json
import os
import subprocess
from pathlib import Path
import shutil
import sys
print(os.getcwd())

phase = sys.argv[1]  # bismsb or ppo
assert phase in ["bismsb", "ppo"], "Phase must be 'bismsb' or 'ppo'"

slurm_task_id = int(os.getenv("SLURM_ARRAY_TASK_ID"))
task_id = slurm_task_id if slurm_task_id is not None else None
if task_id is not None:
    task_id = int(task_id)
else: 
    raise ValueError()
outdir = f"/nfs/disk1/users/jierans/snoplus/{phase}_roi/tagged-mc"
workdir = f"/nfs/disk1/users/jierans/snoplus/{phase}_roi/tagged-mc-working"
Path(workdir).mkdir(parents=True, exist_ok=True)
Path(outdir).mkdir(parents=True, exist_ok=True)

mcinfo = json.load(open(f"./mc_list_{phase}.json"))
directory = mcinfo['directory']
if task_id < len(mcinfo['modules']):
    module = mcinfo['modules'][task_id]
    macro = mcinfo['macro']
else:
    module = mcinfo["coincident_modules"][task_id - len(mcinfo['modules'])]
    macro = mcinfo['coincident_macro']

print("This is host:", os.uname().nodename)
print("SLURM ARRAY ID:", slurm_task_id)
print("Task ID:", task_id)
print("Files to process:", f"{directory}/{module}/*.root")
print("Using macro:", macro)
workfile = f"{workdir}/{module}.ntuple.root"
outfile = f"{outdir}/{module}.ntuple.root"
if Path(f"{directory}/{module}.ntuple.root").is_file():
    input_pattern =f"{directory}/{module}.ntuple.root" 
else:
    input_pattern =f"{directory}/{module}/*.root"
print(f"Input files: {input_pattern}")
print(f"Output file: {outfile}")

subprocess.run([
    "./bin/RunAnalysis", 
    macro,
    "-i", input_pattern,
    "-o", workfile,
])
shutil.move(workfile, outfile)
