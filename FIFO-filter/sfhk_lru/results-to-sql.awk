{

  memory_size = $1;
  misses = $2;

  print "INSERT INTO " policy " (trace_name, memory_size, misses) VALUES ('" trace_name "', " memory_size ", " misses ");";
  
}
