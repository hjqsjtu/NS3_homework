select exp.input, 
       avg(delaydata.value/(1000000.0)) as delay, 
       avg(jitterv.value / (jitterc.value*1000.0)) as jitter 
  from  
  Experiments exp , 
  singletons delaydata, 
  singletons jitterv , 
  singletons jitterc 
  where  
  exp.run = delaydata.run  and
  delaydata.run =jitterv.run and 
  jitterv.run=jitterc.run  and 
  delaydata.variable='delay-average' and 
  jitterv.variable ='jitter-total' and 
  jitterc.variable ='jitter-count' 
   group by exp.input   
   order by abs(exp.input) ASC; 