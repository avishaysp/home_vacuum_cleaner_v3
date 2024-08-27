Authors: Avishay Spitzer & Dory Grossman

# Description
The program simulates

The program recives as an argument a file containing:
1. maximum steps allowed on the run.
2. maximum steps allowed on a signle charge of the battery.
3. Size of the house.
4. map of the house, as the format below.

Writes to the output file the result.

# Algorithm
Since we are certain that our algorithm is very good on avarage, so we dicided that the second algorithm will be similar, and might harness the power of randomness for the second version. They both inherent from the same class.

## Speedom
In general, the algoritm stays and clean the current tile until it's clean or he has to return to charge.
The house is mapped into a graph on the go.
When he finished cleaning and has enough battery, he searches in the graph for the tile that is either dirty or yet to be visited, with the shortest travel distance.
He returns to dock when finished mapping the house and can't clean any more of it, or becouse of max_step.

## Random
Same as 'Speedom', but we use randomness for picking which tile to go to from all the tiles with the minimum distance.

# Example input
```
1. stam house
MaxSteps = 200
MaxBattery=30
Rows = 4
Cols =10
WWWW
W D
W 123456789
```

### The vacuum cleaner see the house as:
```
WWWWWWWWWWWW
WWWWW      W
WW D       W
WW 12345678W
W          W
WWWWWWWWWWWW
```
# Compile

To compile the whole program:

`make all`

To compile first algorithm:

`make 208748665_206476079_speedom_algorithm`

To compile first algorithm:

`make 208748665_206476079_random_speedom_algorithm`

To compile the simulator:

`make simulator`

To clean bin files:

`make clean`

# Run

## Program:

`./myrobot`

## Flags:
set the path for the .so algorithms libs:

`-algo_path=build/algorithm`

set the path for the house maps files:

`-house_path=houses`

set the maximum number of threads:

`-num_threads=5`

disable output file for each run, get only summary.csv:

`-summary_only`

override default and CLI arguments with config file:

`-config=params.config`