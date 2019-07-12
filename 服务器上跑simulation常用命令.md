```
cd /home/liuzhichao/ns3-simulation/
./waf --run "scratch/testBAS --nWifi=25"
./waf --run "scratch/testBAS --cwmin=3 --cwmax=255"
./waf --run "scratch/testBAS --cwmin=7 --cwmax=511"
./waf --run "scratch/testBAS --cwmin=15 --cwmax=1023"
./waf --run "scratch/testBAS --cwmin=31 --cwmax=2047"
./waf --run "scratch/testBAS --cwmin=63 --cwmax=4095"
./waf --run "scratch/testBAS --cwmin=127 --cwmax=8191"
./waf --run "scratch/testBAS --cwmin=255 --cwmax=16383"
./waf --run "scratch/testBAS --cwmin=511 --cwmax=32767"
./waf --run "scratch/testBAS --cwmin=1023 --cwmax=65535"

```

