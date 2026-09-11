# Processing Steps

_This document list all processing steps and the JSON keys they accept in their configuration block._

## AliasStep

Create a new column that aliases (renames) another column.

**JSON keys:**

- `alias`: New alias of the column.
- `name`: Original name of the column.

## CoincidenceStep

Mark _prompt_ and _follower_ tags based on time and spatial coincidence windows. This step considers all prompt candidates, not just the closest ones in time.

**JSON keys:**

- `delayed_expr`: expression to select the delayed event candidates.
- `delta_r_limit`: max distance between prompt and delayed events (Optional. Default is no delta_r limit).
- `detail`: also record the deltaT and deltaR of each event's closest pair (Optional. Default `false`).
- `id_column`: column identifying an event, recorded as the partner reference (Optional. Default is `eventID`).
- `label`: prefix for all columns this step adds.
- `pos_columns`: column names for the X, Y, Z coordinates of event positions (Optional. Default is `posx`, `posy`, and `posz`).
- `prompt_expr`: expression to select the prompt event candidates.
- `time_window`: Max time between a delayed and prompt event.

**Columns added:**

- `<label>_prompt`: tags all prompt events. _All events that pass `prompt_expr` will be tagged as prompt, even if they do not pair with any delayed events._
- `<label>_follower`: tags all delayed events that pair with a prompt.
- `<label>_prompt_eventID`: on follower events, the `id_column` of the pairing prompt closest in time. `-1` otherwise.
- `<label>_follower_eventID`: on prompt events, the `id_column` of the pairing follower closest in time. `-1` otherwise.

With `detail` enabled, four more columns record the separation to that same closest partner, `NaN` where there is none:

- `<label>_prompt_dt`, `<label>_prompt_dr`: on follower events, deltaT (ns) and deltaR to its closest prompt.
- `<label>_follower_dt`, `<label>_follower_dr`: on prompt events, deltaT (ns) and deltaR to its closest follower.

> **Notes:** The pairing is many-to-many — a prompt may have many followers, and a follower may have several prompts. The `_eventID`/`_dt`/`_dr` columns only ever describe the **closest in time** partner, so they are lossy when the fan-out is large. Prompt-side and follower-side columns are kept separate because one event can be both a prompt and a follower. `deltaR` is reported even when no `delta_r_limit` is set (no cut is applied), but `detail` then forces `pos_columns` to be read, which a step without `delta_r_limit` would otherwise skip entirely.

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
