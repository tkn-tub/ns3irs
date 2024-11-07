file = open('irs-validation-sim-100.txt')

data = {}

for line in file.readlines():
    scenario = line.split(' ')[1][:-1]
    throughput = line.split(' ')[3]
    if scenario in data.keys():
        data[scenario].append(throughput)
    else:
        data[scenario] = [throughput]
file.close()

out = open('irs-validation.dat', 'w')
while len(data["LOS"]) > 0:
    out.write(data["LOS"].pop() + " " + data["IRS"].pop() + " " + data["IRSConstructive"].pop() + " 0.0 " + data["MultiIRS"].pop() + "\n")
out.close()
