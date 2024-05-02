#!/bin/bash


graph3() {
    truncate -s 0  graph/graph3speed*
    echo "graph3"
    ./ns3 run assignment -- --graphNumber=3 --seed=3 --fullBufferFlag=true && echo "3 complete"
    ./ns3 run assignment -- --graphNumber=3 --seed=4 --fullBufferFlag=true && echo "4 complete"
    ./ns3 run assignment -- --graphNumber=3 --seed=5 --fullBufferFlag=true && echo "5 complete"
    ./ns3 run assignment -- --graphNumber=3 --seed=6 --fullBufferFlag=true && echo "6 complete"
    ./ns3 run assignment -- --graphNumber=3 --seed=7 --fullBufferFlag=true && echo "7 complete"
    ./ns3 run assignment -- --graphNumber=3 --seed=8 --fullBufferFlag=true && echo "8 complete"
    ./ns3 run assignment -- --graphNumber=3 --seed=9 --fullBufferFlag=true && echo "9 complete"
    ./ns3 run assignment -- --graphNumber=3 --seed=10 --fullBufferFlag=true && echo "10 complete"
    ./ns3 run assignment -- --graphNumber=3 --seed=11 --fullBufferFlag=true && echo "11 complete"
    ./ns3 run assignment -- --graphNumber=3 --seed=12 --fullBufferFlag=true && echo "12 complete"
    ./ns3 run assignment -- --graphNumber=3 --seed=13 --fullBufferFlag=true && echo "13 complete"
    ./ns3 run assignment -- --graphNumber=3 --seed=14 --fullBufferFlag=true && echo "14 complete"
}


graph4() {
    ./ns3 run assignment -- --graphNumber=4 --seed=06 --rngRun=10 --fullBufferFlag=true --scheduler=RR && echo "1 complete" &
    ./ns3 run assignment -- --graphNumber=4 --seed=06 --rngRun=10 --fullBufferFlag=true --scheduler=PF && echo "2 complete" &
    ./ns3 run assignment -- --graphNumber=4 --seed=06 --rngRun=10 --fullBufferFlag=true --scheduler=MT && echo "3 complete" &
    ./ns3 run assignment -- --graphNumber=4 --seed=06 --rngRun=10 --fullBufferFlag=true --scheduler=PSS && echo "4 complete"
}

graph5() {
    ./ns3 run assignment -- --graphNumber=5 --seed=06 --fullBufferFlag=true --scheduler=RR && echo "1 complete" &
    ./ns3 run assignment -- --graphNumber=5 --seed=06 --fullBufferFlag=true --scheduler=PF && echo "2 complete" &
    ./ns3 run assignment -- --graphNumber=5 --seed=06 --fullBufferFlag=true --scheduler=MT && echo "3 complete" &
    ./ns3 run assignment -- --graphNumber=5 --seed=06 --fullBufferFlag=true --scheduler=PSS && echo "4 complete"
}

graph6 () {
    ./ns3 run assignment -- --graphNumber=6 --seed=06 --fullBufferFlag=false --scheduler=RR && echo "1 complete" &
    ./ns3 run assignment -- --graphNumber=6 --seed=06 --fullBufferFlag=false --scheduler=PF && echo "2 complete" &
    ./ns3 run assignment -- --graphNumber=6 --seed=06 --fullBufferFlag=false --scheduler=MT && echo "3 complete" &
    ./ns3 run assignment -- --graphNumber=6 --seed=06 --fullBufferFlag=false --scheduler=PSS && echo "4 complete"

    ./ns3 run assignment -- --graphNumber=7 --seed=06 --fullBufferFlag=false --scheduler=RR && echo "5 complete" &
    ./ns3 run assignment -- --graphNumber=7 --seed=06 --fullBufferFlag=false --scheduler=PF && echo "6 complete" &
    ./ns3 run assignment -- --graphNumber=7 --seed=06 --fullBufferFlag=false --scheduler=MT && echo "7 complete" &
    ./ns3 run assignment -- --graphNumber=7 --seed=06 --fullBufferFlag=false --scheduler=PSS && echo "8 complete"
}


$1
