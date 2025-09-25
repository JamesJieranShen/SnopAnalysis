from pathlib import Path
import sys
import json
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate a JSON list of runs and modules from ROOT files in specified directories.")
    parser.add_argument("-o", "--output", default='.', help='Directory of the output files. Will create jobs.json and tasks.txt')
    parser.add_argument("directories", nargs='+', help="Directories to scan for ROOT files.")
    args = parser.parse_args()
    run_specs = {}
    directories = sys.argv[1:]
    ntasks = 0
    for dd in directories:
        directory = Path(dd)
        files = [f for f in directory.iterdir() if f.is_file() and f.suffix == '.root']
        file_specs = set()
        for file in files:
            fname_no_suffix = file.name.removesuffix('.ntuple.root')
            runstring = fname_no_suffix.split("_")[-3]
            run = int(runstring[1:]) # remove leading 'r'
            module = "_".join(fname_no_suffix.split("_")[:-3])
            file_specs.add(frozenset({
                "module": module, "run": run
            }.items()))
        file_specs = list(file_specs)
        ntasks += len(file_specs)
        file_specs = [dict(f) for f in file_specs]
        file_specs.sort(key=lambda x: (x["module"], x["run"]))
        run_specs[dd] = file_specs
    print(f"Found {ntasks} tasks in {len(directories)} directories.")
    with open(f"{args.output}/jobs.json", 'w') as f:
        json.dump(run_specs, f, indent=2, sort_keys=True)
    print(f"Writing job descriptions to {args.output}/jobs.json")
    with open(f"{args.output}/tasks.txt", 'w') as f:
        for taskid in range(ntasks):
            f.write(str(taskid) + '\n')
    print(f"Writing task list to {args.output}/tasks.txt")
