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
import numpy as np
print(os.getcwd())

slurm_task_id = int(os.getenv("SLURM_ARRAY_TASK_ID"))
task_id = slurm_task_id if slurm_task_id is not None else None
if task_id is not None:
    task_id = int(task_id)
else: 
    raise ValueError()
outdir = "/nfs/disk1/users/jierans/snoplus/bismsb_roi/tagged-mc"
workdir = "/nfs/disk1/users/jierans/snoplus/bismsb_roi/tagged-mc-working"
Path(workdir).mkdir(parents=True, exist_ok=True)
Path(outdir).mkdir(parents=True, exist_ok=True)

mcinfo = json.load(open("./mc_list.json"))
directory = mcinfo['directory']
module = mcinfo['modules'][task_id]

print("This is host:", os.uname().nodename)
print("SLURM ARRAY ID:", slurm_task_id)
print("Task ID:", task_id)
print("Files to process:", f"{directory}/{module}/*.root")
workfile = f"{workdir}/{module}.ntuple.root"
outfile = f"{outdir}/{module}.ntuple.root"
print(f"Output file: {outfile}")

subprocess.run([
    "./bin/RunAnalysis", 
    "./config/tag_mc.json",
    "-i", f"{directory}/{module}/*.root",
    "-o", workfile,
])
shutil.move(workfile, outfile)
