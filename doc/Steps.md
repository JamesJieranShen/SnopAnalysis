# Processing Steps

_This document list all processing steps and the JSON keys they accept in their configuration block._

## AliasStep

Create a new column that aliases (renames) another column.

**JSON keys:**

- `alias`: New alias of the column.
- `name`: Original name of the column.

## CoincidenceStep

Mark _prompt_ and _delayed_ tags based on time and spatial coincidence windows. This step considers all prompt candidates, not just the closest ones in time.

**JSON keys:**

- `delayed_expr`: expression to select the delayed event candidates.
- `delayed_tag_name`: name of the new column that tags all delayed events.
- `delta_r_limit`: max distance between prompt and delayed events (Optional. Default is no delta_r limit).
- `pos_columns`: column names for the X, Y, Z coordinates of event positions (Optional. Default is `posx`, `posy`, and `posz`).
- `prompt_expr`: expression to select the prompt event candidates.
- `prompt_tag_name`: name of the new column that tags all prompt events. _All events that passes `prompt_expr` will be tagged as prompt, event if they do not pair with any delayed events._
- `time_column`: Column to use as the time of the event.
- `time_window`: Max time between a delayed and prompt event.

## DefineStep

Define a new column from an expression evaluated by RDataFrame.

**JSON keys:**

- `expr`: expression to evaluate for the new column.
- `name`: name of the new column.

## DeltaTStep

Add a per-entry time-difference column computed from a rolling 50 MHz counter (`clockCount50`).

**JSON keys:**

- `name`: name of the new column (default is `delta_t`).

> **Notes:** Uses `clockCount50` and a 50 MHz rollover (20 ns ticks).

## DisplayStep

Print column names and inferred ROOT types to stdout; passthrough.

_This Step does not take any configurable values._

## FilterStep

Filter rows by a boolean expression; optionally name the filter.

**JSON keys:**

- `expr`: expression to filter on.
- `name`: name for the filter (Optional -- only useful for inspecting the RDataFrame directly).

## ResetStep

Reset the pipeline to an empty DataFrame and (optionally) toggle ROOT ImplicitMT on/off.

**JSON keys:**

- `multithreading`: Turn ImpliticMT on or off (Optional. Not specifying this flag will result in the ImpliticMT state not being altered).
