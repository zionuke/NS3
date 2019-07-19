import re

pat = re.compile(
    r'(.*?)ns UE:(\d+)at.*?CQI:(\d+)current throughput(.*?)avg throughput(.*?)$')
info = [[], [], [], [], []]
with open('5_1_1800', 'r') as f:
    for line in f.readlines():
        mat = pat.match(line)
        if mat:
            info[int(mat.group(2))].append((float(mat.group(1))/1e6,
                                            int(mat.group(3)), float(mat.group(4))))

for u in range(1, 5):
    tm = 0
    print('UE ', u)
    for t, c, p in info[u]:
        if t > tm:
            tm += 0.5
            print(t, c)
