echo -off
fs0:

if %1 == -start then
  set recount 0
endif

set flag 0

echo "*%HReset %V %recount% %H times%N"
echo "*%HReset %V %recount% %H times%N" > Reboot.log

for %a run (%recount% 10000)
  if %a == %recount% then
    set flag 1
  else
    if %flag% == 1 then
      echo "%HPress %V'ctrl + c'%H to stop the script%N"
      set recount %a
      for %b run (3 1 -1)
        echo "reset in %V %b %N seconds"
        stall 1000000
      endfor
      echo -on
      reset
    endif
  endif
endfor

echo "%HTest finished"
set -d flag
set -d recount
echo -on
