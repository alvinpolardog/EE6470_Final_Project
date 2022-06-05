# EE6470 Final Project Report
吳哲廷 學號:110061590

Link to [Github Repo](https://github.com/alvinpolardog/EE6470_Final_Project)
##

## Implementation of Multiple Accelerator PEs on a Multi-Core RISCV-VP platform

For the final project, I chose to continue my midterm project, and improve upon Nonlinear Energy Operator, an algorithm commonly used in neural engineering to pickup
spikes in brain activity. In the final project, the algorithm was optimized at the module level, and parallelized using multiple RISCV cores.
##  
## 

### Algorithm Introduction 

NEO or Nonlinear Energy Operator, refers to the following calculation:


![formula](https://i.imgur.com/469F8zA.png)

The main purpose of this calculation is to accentuate high-frequency content, and emphasize instantaneous changes. This is essential in detecting anomaly in EEG, where signals are often very tightly packed, making some spikes difficult to distinguish.

In this following example image, we can see a simulated signals containing multiple spikes. However it may be difficult to simply choose a threshold and find all the peaks, since the base of the signal is constantly fluctuating. In the NEO version, the signal becomes much more clear, and we can quickly differentiate between significant spikes and noise.


![NEO example image](https://i.imgur.com/s6QUo4z.png)

In the midterm project, I used NEO as the kernel function, and created a module that can process any single channel electroencephalogram easily and quickly, irregardless of length.

But another critical part of the spike detection process is deciding a robust threshold. One method is to choose a static amplitude, but this can often lead to false positive when the signal becomes overly noisy. 
In the final project, I will implement an adaptive threshold as a pipelined module, allowing the system to output a detected spike mask instead of just the NEO results.

![](https://i.imgur.com/KMIqDYn.png)

### High Level Synthesis of Module-Level Pipelining
![Original Module Layout](https://user-images.githubusercontent.com/93983804/172034868-d194bae0-a26b-4c35-ae86-0202f957724c.png)
Above is the original design for the NEO module as part of the midterm project. This was the 4th version, and was optimized by unrolling the loops
used for data reading and NEO calculation. This pipelined-optimized module resulted in a 90% reduction in simulation time with just double the area.

In the final project, since the NEO module was already as optimized as I could create, I directly ported it into the new Spike Detector System.
This new system consist of two NEO modules in parallel, connecting to a new thresholding module via FIFO channels. 
![image](https://user-images.githubusercontent.com/93983804/172035039-eea03879-7c38-4157-a3f2-8585ea2d7eab.png)

The thresholding module accumulated the input NEO values, and solve for a thresholding value based on the rolling mean. Each of the input NEO value is then
compared to the threshold value individually, and the ones that exceeded it are marked and output. 

#### HLS Optimization Comparison
In high level synthesis, since the NEO module has already been optimized, I had instead modified the buffer size of the NEO module and the thresholding module to see
how it may affect runtime and area.

| Optimization | Total Runtime (ns) | Time Difference | Area | Area Difference | Average Latency (Cycles) |
| -------- | -------- | -------- | ------ | ------ | ---- |
| Original (BS = 4) | 30010     | *benchmark*   | 3301.8  | *benchmark* | 7|
| DPA (BS = 4)      | 30010     | ±0%           | 3080.8  | -6.7%     | 7|
| Buffer Size = 5   | 19990     |-33.3%         | 4796.0  | +45.2%    | 5|
| DPA (BS = 5)      | 19990     |-33.3%         | 4464.6  | +35.2%    | 5|
| Buffer Size = 6   | 15010     | -49.9%        | 6290.2  | +90.5%    | 4|
| DPA (BS = 6)      | 15010     | -49.9%        | 5848.3  | +77.1%    | 4|

The result shows a clear correlation between the buffer size and the resulting parameters. With a larger buffer size, there is less redundancy in the transfered data as well as a higher amount of data each transfer carries, these lead to lower amount of transfers for the same dataset, and thus improving runtime but at the cost of area. 


### RISCV VP Simulation
For RISCV VP simulation, I first partition the input signal according to the number of cores. In this example, I chose 2 cores, thus the signal is cut in half. However, since the signal around the cut will be lost due to the nature of NEO, the left partition will always overlap the right partition a few extra samples. This redundancy allows the cut signal to still be examined by at least one core for spikes.

![image](https://user-images.githubusercontent.com/93983804/172044464-f11f2db4-4a0f-49ca-9c46-0242f89a847f.png)

The architecture of the actual RISCV VP simulation is as follows:
![image](https://user-images.githubusercontent.com/93983804/172044486-a488ec4a-2c51-46c9-b6ff-f756378faf45.png)

Each RISCV core will be connected to one PE, and will control the data transfer between them for 
the respective data partition. The first core (hartid==0) will be responsible for reading the stimulus data into the global memory as well as outputting the result onto a file.

To prevent the cores from transfering data to the detector system before the input data is read or after the result is being output, synchronization barriers are placed before and after the data transfering stage.

One key note in using multiple cores is not allowing multiple core to read/write into the same location at a single period of time, that is, race conditions. This problem does not affect the NEO resulting output, since the output is only written when there is a spike, and which core is writing to it does not matter. The place where the problem does materialize is at the DMA. Since DMA requires multiple location to be written to in one consecutive write, it does not work well with multiple cores, at least without any modification. To fix this issue, a mutex was used so that only one core can access the DMA at one time.

The following is the result of RISCV VP simulation:
| Configuration | Simulation Time (ns) | num-instru (Core 1) |num-instru (Core 2) |
| -------- | -------- | -------- | ------ |
| Single Core Single PE | 134931100 | 3612849   || 
| Dual Core Dual PE | 76764550     | 2241097     | 3339491 | 


Below is the visualization of the final result of the spike mask generated from the spike detector.

![image](https://user-images.githubusercontent.com/93983804/172045417-1df1dc56-f17f-497f-bde9-c107968dddb8.png)

The blue line shown is the synthesized input signal, while the black dots are the output spike mask of the system.

##  

## Final Conlusion

In the final project, it is shown that with the HLS directives optmization as well as module-level parallelization, we can achieve very good and effective result in NEO. By modifying the buffer sizes of the modules, we can also pick the perfect balance between speed and space, depending on our needs.

With the RISCV VP simulation, we can see the NEO is very fit for multicore parallelization. The main reason is that the partitioning of the input signal is very simple, at least when the input signal is not a real-time input. This allows the core to work independently, drastically reducing the simulation time.

From the midterm and final project, it is clear that HLS and RISCV VP provides a very fast way to test algorithms on their synthesizability as well as how they may run with RISCV cores. With my topic of spike detection with NEO, it is also clear that algorithms with simple kernel function work escpecially well for acceleration using PE.

In the future, I hope to use these techniques on more complicated algorithms, and see more of what HLS and multicore systems are capable of.
