# Extension of the ns3 Network Simulator to Support Intelligent Reflecting Surfaces

## User Guide
### Prerequisites
Before using the IRS module, ensure the following requirements are met:

- Tested with `ns-3.42`
- Module located in the `contrib/` directory
- `Eigen3` library installed, and ns-3 built with the `--enable-eigen` flag.
  [Eigen3 Documentation](https://gitlab.com/libeigen/eigen)
- The modified ns-3 installation is at [jakob.ruehlow/ns-3-dev](https://webgit.ccs-labs.org/git/jakob.ruehlow/ns-3-dev)
- When using your own ns-3 installation, modify the following:

  ```cpp
  // src/network/helper/node-container.h:
  // From:
  class NodeContainer;
  // To:
  class NodeContainer : public Object;
  ```
### Using the IRS Module in Simulations
#### 1. Creating the IRS Node
Create one or multiple IRS nodes using `NodeContainer`:
```cpp
NodeContainer irsNodes;
irsNodes.Create(1);
```

#### 2. Configuring Node Mobility
The IRS node(s) require a mobility model, which can be installed using the `MobilityHelper`, just like any other node in ns3.
While any mobility model can theoretically be applied, it is recommended to use a static mobility model for the IRS in multi-IRS scenarios.
In these scenarios moving IRS nodes can lead to inaccurate results, as the optimization strategies for the possible paths with more than one IRS assume stationary IRS.
```cpp
MobilityHelper mobility;
Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
positionAlloc->Add({0, 0, 0});
mobility.SetPositionAllocator(positionAlloc);
mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
mobility.Install(irsNodes);
```

#### 3.1 Configuring the IRS Module: IrsLookupModel
Use the `IrsLookupHelper` to configure the IRS node with an `IrsLookupModel`, utilizing a pre-generated csv file for the lookup data:
```cpp
IrsLookupHelper irsHelper;
irsHelper.SetDirection({0, 1, 0});
irsHelper.SetLookupTable("path/to/lookup_table.csv");
irsHelper.Install(irsNodes);
```
The lookup table can be generated using the `generateIrsLookupTable` helper script in the `matlab/` directory, as follows:
```matlab
freq = 5.15e9;		  % Frequency in Hz
Nr = 20;		        % Number of elements in rows
Nc = 20;		        % Number of elements in columns
ap = [0;0;0];		    % AP position
ue = [50;0;0];		  % UE position
ris = [0.7;-0.7;0];	% IRS position
dir = [0,1,0];	    % IRS direction
[r_ap_ris, a_ap_ris, r_ris_ue, a_ris_ue] = calcangle(ap, ue, ris, dir);
ris_table = generateIrsLookupTable( ...
    round(a_ap_ris(1)), round(a_ris_ue(1)), Nr, Nc, freq, ...
    abs(ue(1) - ap(1)), r_ap_ris, r_ris_ue, 0, "constructive" ...
);
```

#### 3.2 Configuring the IRS Module: IrsSpectrumModel
Using the `IrsSpectrumModel`, the IRS node can be configured as follows:
```cpp
Ptr<IrsSpectrumModel> irs = CreateObjectWithAttributes<IrsSpectrumModel>(
    "Direction",
    VectorValue({0, 1, 0}),
    "N",
    TupleValue<UintegerValue, UintegerValue>({20,20}),
    "Spacing",
    TupleValue<DoubleValue, DoubleValue>({0.05, 0.05}),
    "Samples",
    UintegerValue(100),
    "Frequency",
    DoubleValue(5.21e9));
irs->CalcRCoeffs(los_distance,
		 irs_distance,
		 Angles(in_az, in_el),
		 Angles(out_az, out_el),
		 0);
irsNodes.Get(0)->AggregateObject(irs);
```

`los\_distance` refers to the length of the LOS path in meters, while `irs\_distance` represents the distance of the path reflected over the IRS in meters.
The variables `in\_az` and `in\_el` represent the optimized incoming azimuth and elevation angles, respectively, in radians.
Similarly, `out\_az` and `out\_el` correspond to the optimized outgoing azimuth and elevation angles, respectively, in radians.
The last argument in the `CalcRCoeffs` function is set to zero, which calculates the reflection coefficients so they create constructive interference with the LOS path.

*N* represents the number of elements in both the row and column directions, while *Spacing* denotes the distance between elements. *Samples* specifies the number of samples taken of the radio wave, and *Frequency* indicates the operating frequency for which the IRS is designed.

## Simulations Conducted in this Thesis
### File Structure
```
.
├── README.md
├── examples
│   ├── hidden-node-problem.cc
│   ├── irs-optimal-placement.cc
│   ├── irs-validation.cc
├── results_and_scripts
│   ├── hidden_node_problem.plt
│   ├── irs-validation.plt
│   ├── optimal-irs-placement.plt
│   └── validation-processing.py
```

### Simulation Results and Data Processing
**Ns3 must be compiled with the `--enable-examples` and `--enable-eigen` flags for the following to work.**

#### **Figure 4.2: IRS Validation Results**
> Simulation results for the scenario in Figure 4.1, showing SNR relative to IRS position. The IRS moves from near Tx to near Rx. Given a transmit power of P_Tx = 17 dB and a noise power of P_N = -94dB.

```bash
$ ns3 run irs-optimal-placement
```
Import generated csv into matlab then use `matlab/optimalIrsPlacement.m` to add ns3, matlab and etsi results to a new csv.
```bash
# cd contrib/irs/results_and_scripts/
$ gnuplot optimal-irs-placement.plt
```

#### **Table 4.1: IRS Validation Results**
Ns3 results are retrieved as shown below.
Matlab results are simulated in `matlab/validationDifferentScenarios.m`

#### **Figure 4.9 &  Table 4.2: IRS Validation Results**
>Simulation results for the validation scenarios, showing throughput for different configurations: (1) LOS, (2) only IRS, (3) IRS and LOS with constructive interference, (4) IRS and LOS with destructive interference, and (5) two IRS creating constructive interference with the LOS path.

>Mean values for the validation scenarios, presenting throughput, SNR, data rate, and success rate.

```bash
$ parallel ./ns3 run "'irs-validation --scenario={1} --run={2}'" --no-build --quiet ::: LOS IRS IrsConstructive IrsDestructive multiIrs ::: $(seq 1 100) > contrib/irs/results_and_scripts/irs-validation-sim-100.txt

# cd contrib/irs/results_and_scripts/
$ python validation-processing.py

$ gnuplot irs-validation.plt
```

#### **Figure 4.12: Evaluation Results**
> Simulation results for the hidden terminal problem, showing throughput over time for three different configurations: (1) base case with only nodes A and C, (2) with RTS/CTS enabled, and (3) with IRS.

```bash
$ ns3 run "hidden-node-problem --rts-cts"

# cd contrib/irs/results_and_scripts/
$ gnuplot hidden_node_problem.plt
```
Results are saved as `hidden_node_problem_throughput.plt`, but the correct formatting is in `hidden_node_problem.plt`
