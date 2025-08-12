# Output Writers

_This document list all steps that write the RDataFrame out to something._

## SnapshotWriter

Create a RDataFrame using a TChain

**JSON keys:**

- `tree_name`: Name of the TTree to write to.
- `file_name`: Name of the file to write to. If the file already exists, it will be overwritten.
- `columns`: List of column names to write. If not specified, all columns will be written.
