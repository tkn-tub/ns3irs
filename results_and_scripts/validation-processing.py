import statistics
from collections import defaultdict

with open('irs-validation-sim-100.txt') as file:
    data = defaultdict(lambda: [[], [], [], []])

    for line in file:
        parts = line.split(' ')
        scenario = parts[1][:-1]
        throughput = float(parts[3])
        snr = float(parts[6])
        datarate = float(parts[10])
        successrate = float(parts[13].split(':')[1][:-2])

        data[scenario][0].append(throughput)
        data[scenario][1].append(snr)
        data[scenario][2].append(datarate)
        data[scenario][3].append(successrate)

for scenario, values in data.items():
    print(f'[{scenario}] Throughput: {statistics.mean(values[0]):.2f} Mbps, '
          f'SNR: {statistics.mean(values[1]):.2f} dB, '
          f'Data Rate: {statistics.mean(values[2]):.2f} Mbps, '
          f'Success Rate: {statistics.mean(values[3]):.2f}%')

with open('irs-validation.dat', 'w') as out:
    num_lines = len(data["LOS"][0])

    for i in range(num_lines):
        los_value = data["LOS"][0][i]
        irs_value = data["IRS"][0][i]
        irs_constructive_value = data["IRSConstructive"][0][i]
        irs_destructive_value = data["IRSDestructive"][0][i]
        multi_irs_value = data["MultiIRS"][0][i]

        out.write(f'{los_value} {irs_value} {irs_constructive_value} {irs_destructive_value} {multi_irs_value}\n')


