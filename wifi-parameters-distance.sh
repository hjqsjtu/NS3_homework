#!/bin/sh

#DISTANCES="25 50 75 100 125 145 147 150 152 155 157 160 162 165 167 170 172 175 177 180"
DISTANCES="25 50 75 100 125 145  150  155  160  165  170  175  180"
#SLOT="3 4 5 6 7 8 9 10 11 12 14 15 16 20"
TRIALS="1 2 3 4 5"

echo WiFi Experiment Parameters Tests

pCheck=`which sqlite`
if [ -z "$pCheck" ]
then
  echo "ERROR: This script requires sqlite3 (wifi-example-sim does not)."
  exit 255
fi

pCheck=`which gnuplot5`
if [ -z "$pCheck" ]
then
  echo "ERROR: This script requires gnuplot (wifi-example-sim does not)."
  exit 255
fi

pCheck=`which sed`
if [ -z "$pCheck" ]
then
  echo "ERROR: This script requires sed (wifi-timing-attributes does not)."
  exit 255
fi

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:bin/

if [ -e ../../mydata.db ]
then
  #echo "Kill mydata.db? (y/n)"
  #read ANS
  #if [ "$ANS" = "yes" -o "$ANS" = "y" ]
  #then
    echo Deleting database
    sudo rm ../../mydata.db
  #fi
fi

for trial in $TRIALS
do
  for distances in $DISTANCES
  do
    echo Trial $trial, distances $distances
    sudo ../../waf --run "wifi-timing-attributes --format=db --distance=$distances --run=run-$distances-$trial"
  done
done

#
#Another SQL command which just collects raw numbers of frames received.
#
#CMD="select Experiments.input,avg(Singletons.value) \
#    from Singletons,Experiments \
#    where Singletons.run = Experiments.run AND \
#          Singletons.name='wifi-rx-frames' \
#    group by Experiments.input \
#    order by abs(Experiments.input) ASC;"
if [ -e mydata.db ]
then
    #echo Deleting current mydata.db
    sudo rm -rf mydata.db
fi
mv ../../mydata.db .

#Packet loss 
#CMD="select exp.input,avg(100-((rx.value*100)/tx.value)) \
#    from Singletons rx, Singletons tx, Experiments exp \
#    where rx.run = tx.run AND \
#          rx.run = exp.run AND \
#          rx.variable='receiver-rx-packets' AND \
#          tx.variable='sender-tx-packets' \
#    group by exp.input \
#    order by abs(exp.input) ASC;"

CMD="select exp.input, \
       avg((rdata.value*packets.value*8)/((lmax.value-lmin.value)/1000.0)) as Throughput, \
       avg(100-((receiver.value*100)/sender.value))   \
  from  \
  Experiments exp , \
  singletons lmax , \
  singletons lmin , \
  singletons rdata, \
  singletons packets , \
  singletons sender , \
  singletons receiver  \
  where  \
  packets.run=exp.run and \
  packets.run=rdata.run and \
  rdata.run=lmax.run and  \
  lmax.run=lmin.run and \
  lmin.run=sender.run and \
  sender.run=receiver.run and \
  receiver.run=rdata.run and  \
  rdata.variable='receiver-rx-packets' and \
  lmax.variable='last-max' and \
  lmin.variable='last-min' and \
  packets.variable='tx-pkt-size-max' and \
  sender.variable='sender-tx-packets' and \
  receiver.variable='receiver-rx-packets' \
   group by exp.input   \
   order by abs(exp.input) ASC; "


#below is delayjitter statistics statements;
#select jt.value/jc.value from singletons jc,singletons jt where jc.run=jt.run and jc.run='run-1555588380' and jc.variable='jitter-count' and jt.variable='jitter-total';

#below is delay statistics statements;
#select dd.value/1000.0 from singletons dd where dd.variable='delay-average' and dd.run='run-1555588380';
if [ -e wifi-result1.data ]
then
    echo Deleting wifi-result1.data
    sudo rm -rf wifi-result1.data
fi
sqlite3 -noheader mydata.db "$CMD" > wifi-result1.data
sed -i.bak "s/|/   /g" wifi-result1.data
rm wifi-result1.data.bak


CMD="select exp.input, \
       avg(delaydata.value/(1000000.0)) as delay, \
       avg(jitterv.value / (jitterc.value*1000.0)) as jitter \
  from  
  Experiments exp , \
  singletons delaydata, \
  singletons jitterv , \
  singletons jitterc \
  where  \
  exp.run = delaydata.run  and \
  delaydata.run =jitterv.run and \
  jitterv.run=jitterc.run  and \
  delaydata.variable='delay-average' and \
  jitterv.variable ='jitter-total' and \
  jitterc.variable ='jitter-count' \
   group by exp.input   \
   order by abs(exp.input) ASC; "

if [ -e wifi-result2.data ]
then
    echo Deleting wifi-result2.data
    sudo rm -rf wifi-result2.data
fi
sqlite3 -noheader mydata.db "$CMD" > wifi-result2.data
sed -i.bak "s/|/   /g" wifi-result2.data
rm wifi-result2.data.bak

gnuplot5 wifi-parameters-distance.gnuplot

echo "Done; data in wifi-result.data, plot in wifi-result.eps"
