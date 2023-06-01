for (( i=2; i <= 10; ++i ))
 do
   printf "%s, %s, %s, %s\n" "$(date '+%T')" "$(smtc 0 read 1)" "$(smtc 0 read 2)" "$(smtc 0 read 3)" >> temperatures.csv
   sleep 1
  done
