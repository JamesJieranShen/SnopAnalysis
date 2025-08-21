from pathlib import Path
import sys
import json

if __name__ == "__main__":
    if len(sys.argv) < 2 or sys.argv[1] == "--help":
        print("Usage: python run_list.py <directory>")
        sys.exit(0)
    run_specs = {}
    directories = sys.argv[1:]
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
        file_specs = [dict(f) for f in file_specs]
        file_specs.sort(key=lambda x: (x["module"], x["run"]))
        run_specs[dd] = file_specs
    json.dump(run_specs, sys.stdout, indent=2, sort_keys=True)
