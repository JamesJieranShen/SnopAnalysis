# Input Providers

_This document list all input providers that provide a RDataFrame for further processing._

## TChainProvider

Create a RDataFrame using a TChain

**JSON keys:**

- `parts`: a list of JSON objects. Each object represents a part of the TChain. The first "part" will be used to create the main TChain, while thr reset will be added as TChain friends (additional branches).
  Each part contains the following keys:
- `tree_name`: name of the TTree to use in this part.
- `files`: a list of patterns that describe the files that will be used in this part. Contents of this list are passed to `TChain::AddFile` -- so both file names and wildcard patterns are allowed.
- `sort`: If true, the files in this part will be sorted lexicographically before being added to the final TChain. Default is `false`.

## Filelistprovider

Create a RDataFrame using a list of files.

**JSON keys:**

- `tree_name`: Name of the TTree to use.
- `files`: A list of file names or patterns that describe the files to use. File will be added by the order they appear in the list.
