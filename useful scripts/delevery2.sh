cnt=0; 
for file in $(find ./ -type f); #<-- if not too many matches
do 
  let cnt=cnt+1; 
  if [ $cnt -eq 2 ]; 
    then rm $file;               #<-- do something
    cnt=0;                         #<-- alternate file
  fi; 
done
