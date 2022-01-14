#!/usr/bin/awk -f

{
  size = 30
  
  pageFault = 0
  interrupt = 0
  write = 0

  for (i=0; i<30*5; i+=5){
    pageFault += $(i+3)
    interrupt += $(i+4)
    write += $(i+5)
  }
  
  pageFault /= size
  interrupt /= size
  write /= size
  print $1 "," $2 ","  pageFault "," interrupt "," write
}
