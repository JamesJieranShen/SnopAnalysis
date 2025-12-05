import json
import os
from pathlib import Path
import uproot
import argparse
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Validate tagged output files")
    parser.add_argument("outdir", type=str)
    args = parser.parse_args()
    outdir = args.outdir
    if not os.path.isdir(outdir):
        print(f"Output directory {outdir} does not exist!")
        exit(1)

    run_list = json.load(open("runs.json"))
    with open("tasks.txt") as f:
        tasks = f.readlines()
        tasks = [int(t.strip()) for t in tasks]
    flat_list = []
    for directory, filespec in run_list.items():
        for f in filespec:
            f["directory"] = directory
            flat_list.append(f)

    remaining_tasks = []
    for taskid in tasks:
        run_spec = flat_list[taskid]
        outfile = f"{outdir}/{run_spec['module']}_r{run_spec['run']:010d}.ntuple.root"
        is_good = Path(outfile).is_file()
        if is_good:
            try:
                rootfile = uproot.open(outfile)
                tree = rootfile['output']
            except Exception as _:
                is_good = False
        print(f"{'\033[32m' if is_good else '\033[31m'}Output file: {outfile}: {'OK' if is_good else 'BAD'}\033[0m")
        if not is_good:
            remaining_tasks.append(taskid)
    print("Remaining tasks:", remaining_tasks)
    answer = input("Overwrite tasks.txt?[y/N]")
    if answer.lower() in ["y","yes"]:
        with open("tasks.txt","w") as f:
            for t in remaining_tasks:
                f.write(f"{t}\n")
    else:
        print("Not overwriting tasks.txt")

