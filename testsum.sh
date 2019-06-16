#!/bin/bash

if [ $# -ne 1 ]; then
    echo usage: $(basename $0) pidServer
    exit 1
fi

exec 4<testout.log

clientLanciati=0
tipoUnoLanciati=0
tipoDueLanciati=0
tipoTreLanciati=0

testUno=0
testDue=0
testTre=0

successiUno=0
successiDue=0
successiTre=0

fallimentiUno=0
fallimentiDue=0
fallimentiTre=0

while read -u 4 -r line; do
    temp=( $line )
    if [ ${temp[0]} == 1 ]; then
        tipoUnoLanciati=$(( tipoUnoLanciati + 1 ))
        testUno=$(expr $testUno + ${temp[1]})
        successiUno=$(expr $successiUno + ${temp[2]})
        fallimentiUno=$(expr $fallimentiUno + ${temp[3]})
    fi

    if [ ${temp[0]} == 2 ]; then
        tipoDueLanciati=$(( tipoDueLanciati + 1 ))
        testDue=$(expr $testDue + ${temp[1]})
        successiDue=$(expr $successiDue + ${temp[2]})
        fallimentiDue=$(expr $fallimentiDue + ${temp[3]})
    fi

    if [ ${temp[0]} == 3 ]; then
        tipoTreLanciati=$(( tipoTreLanciati + 1 ))
        testTre=$(expr $testTre + ${temp[1]})
        successiTre=$(expr $successiTre + ${temp[2]})
        fallimentiTre=$(expr $fallimentiTre + ${temp[3]})
    fi 

    clientLanciati=$(( clientLanciati + 1 ))
done

echo "Client lanciati: $clientLanciati di cui tipo 1: $tipoUnoLanciati, tipo 2: $tipoDueLanciati, tipo 3: $tipoTreLanciati"
echo "Numero test di tipo 1 lanciati: $testUno di cui $successiUno successi e $fallimentiUno fallimenti"
echo "Numero test di tipo 2 lanciati: $testDue di cui $successiDue successi e $fallimentiDue fallimenti"
echo "Numero test di tipo 3 lanciati: $testTre di cui $successiTre successi e $fallimentiTre fallimenti"

kill -USR1 $1