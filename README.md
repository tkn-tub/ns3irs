# ns3irs: Simulating Intelligent Reflecting Surfaces in ns-3

## User Guide
### Prerequisites
Before using the IRS module, ensure the following requirements are met:

- Tested with `ns-3.43`
- This module has to be located in the `contrib/` directory
- The [Eigen3 library](https://gitlab.com/libeigen/eigen) must be installed, and ns-3 must be built with the `--enable-eigen` flag.
- A modification is required in the ns-3 source code:
```cpp
// File: src/network/helper/node-container.h
// Original:
class NodeContainer;
// Replace with:
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
freq = 5.15e9;      % Frequency in Hz
Nr = 20;            % Number of elements in rows
Nc = 20;            % Number of elements in columns
ap = [0;0;0];       % AP position
ue = [50;0;0];      % UE position
ris = [0.7;-0.7;0]; % IRS position
dir = [0,1,0];      % IRS direction
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
    "Frequency",
    DoubleValue(5.21e9));
irs->CalcRCoeffs(los_distance,
         irs_distance,
         Angles(in_az, in_el),
         Angles(out_az, out_el),
         0);
irsNodes.Get(0)->AggregateObject(irs);
```

`los_distance` refers to the length of the LOS path in meters, while `irs_distance` represents the distance of the path reflected over the IRS in meters.
The variables `in_az` and `in_el` represent the optimized incoming azimuth and elevation angles, respectively, in radians.
Similarly, `out_az` and `out_el` correspond to the optimized outgoing azimuth and elevation angles, respectively, in radians.
The last argument in the `CalcRCoeffs` function is set to zero, which calculates the reflection coefficients so they create constructive interference with the LOS path.

*N* represents the number of elements in both the row and column directions, while *Spacing* denotes the distance between elements. *Frequency* indicates the operating frequency for which the IRS is designed.
