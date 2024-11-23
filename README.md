# *Extension of the ns3 Network Simulator to Support Intelligent Reflecting Surfaces*

## Simulations Used in the Thesis
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

#### **Figure 4.2: IRS Validation Results**
> Simulation results for the scenario in Figure 4.1, showing SNR relative to IRS position. The IRS moves from near Tx to near Rx. Given a transmit power of P_{Tx} = 17 dB and a noise power of P_N = -94dB.

```bash
$ ns3 run irs-optimal-placement
```
Import generated csv into matlab then use `matlab/optimalIrsPlacement.m` to add ns3, matlab and etsi results to a new csv.
```bash
# cd contrib/irs/results_and_scripts/
$ gnuplot optimal-irs-placement.plt
```

#### **Table 4.1: IRS Validation Results**
ns3 results are retrieved as below.
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
