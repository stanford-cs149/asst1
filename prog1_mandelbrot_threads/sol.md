All experiments are done of machine with the following configurations:

{
      "_name" : "hardware_overview",
      "activation_lock_status" : "activation_lock_enabled",
      "boot_rom_version" : "11881.121.1",
      "chip_type" : "Apple M4",
      "machine_model" : "Mac16,12",
      "machine_name" : "MacBook Air",
      "model_number" : "MW133HN/A",
      "number_processors" : "proc 10:4:6",
      "os_loader_version" : "11881.121.1",
      "physical_memory" : "16 GB",
    }


Approach 1:
Parallelising parts of image by splitting rows

View 1
-> 1.98x speedup from 2 threads
-> 1.58x speedup from 3 threads
-> 2.36x speedup from 4 threads
-> 2.31x speedup from 5 threads
-> 3.04x speedup from 6 threads
-> 3.14x speedup from 7 threads
-> 3.53x speedup from 8 threads

View 2
-> 1.64x speedup from 2 threads
-> 1.97x speedup from 3 threads
-> 2.29x speedup from 4 threads
-> 2.60x speedup from 5 threads
-> 2.94x speedup from 6 threads
-> 3.28x speedup from 7 threads
-> 3.47x speedup from 8 threads

work is split unevenly among different chunks of the image
do a row by row assignment of threads instead

View 1
-> 1.99x speedup from 2 threads
-> 2.67x speedup from 3 threads
-> 3.55x speedup from 4 threads  
-> 3.93x speedup from 5 threads
-> 4.38x speedup from 6 threads
-> 4.88x speedup from 7 threads
-> 5.35x speedup from 8 threads