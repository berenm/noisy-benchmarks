BEGIN {
  time = 0;
  last = 0;
  total = 0;
  minimum  =0;
  maximum = 0;
  count = 0;
}

{
  type  = $1;
  idx   = $2;
  value = int($3);
  time += value;
  current = int(time / granularity) * granularity;

  if (last == 0 || current != last) {
    if(last > 0)
      print type, last, count, total, total / count, minimum, maximum;

    count = 1;
    total = value;
    minimum = value;
    maximum = value;
  }
  else
  {
    count += 1;
    total += value;
    if(value < minimum)
      minimum = value;
    if(value > maximum)
      maximum = value;
  }

  last = current;
}