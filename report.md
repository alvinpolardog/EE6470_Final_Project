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

| Optimization | Total Runtime (ns) | Time Difference | Area | Area Difference | Average Latency (Cycles) |
| -------- | -------- | -------- | ------ | ------ | ---- |
| Original (BS = 4) | 30010     | *benchmark*   | 3301.8  | *benchmark* | 7|
| DPA (BS = 4)      | 30010     | ±0%           | 3080.8  | -6.7%     | 7|
| Buffer Size = 5   | 19990     |-33.3%         | 4796.0  | +45.2%    | 5|
| DPA (BS = 5)      | 19990     |-33.3%         | 4464.6  | +35.2%    | 5|
| Buffer Size = 6   | 15010     | -49.9%        | 6290.2  | +90.5%    | 4|
| DPA (BS = 6)      | 15010     | -49.9%        | 5848.3  | +77.1%    | 4|



### RISCV VP Simulation



##  

## Final Conlusion

Take a look at the full comparison.

| Optimization | Total Runtime (ns) | Time Difference | Area | Area Difference | Average Latency (Cycles) |
| -------- | -------- | -------- | ------ | ------ | ---- |
| [V1] No Optimization | 119950 | *benchmark*   | **2969.1** | *benchmark* | 5|
| [V1] DPOPT    | 119950     | ±0%     | 3358.0 | +13% | 5|
| [V1] LOOP_UNROLLING-4     | 134950     |+12%| 4360.5     | +46% | 5|
| [V1] LOOP_UNROLLING-8     | 139950     |+12%| 5311.5     | +78% | 5|
| [V1] PIPELINE-1     | 20040     | -83%     | 4532.4| +52% | 3|
| [V1] PIPELINE-1 DPOPT   | 20040     | -83%     | 4150.4 | +39% | 3|
| [V1] PIPELINE-2     | 40030     | -67%     | 3973.8 | +34% | 4|
| [V1] PIPELINE-2 DPOPT  | 40030     | -67%     | 4208.8 | +41% | 4|
| [V2] No Optimization | 113730 | -5%   | 14542.7| +389% | 89|
| [V2] DPOPT    | 78050     | -35%     | 12515.8| +321% | 61|
| [V2] LOOP_UNROLLING-2     | 113730     |-5%| 21346.5    | +618% | 89|
| [V2] LOOP_UNROLLING-2 DPOPT     | 86970     |-38%| 17725.7     | +497% | 68|
| [V2] PIPELINE-1     |42370     | -65%     | 16040.4 | +440% | 32|
| [V2] PIPELINE-1 DPOPT   |42370     | -65%     | 9947.7 | +235% | 32|
| [V2] PIPELINE-2 DPOPT  | 60210     | -50%     | 11368.7 | +282% | 47|
| [V3] BANDWIDTH = 2 (PIPELINE-1 DPOPT)  | 50060     | -58%      | 5305.8| +79% | 8|
| [V3] BANDWIDTH = 3 (PIPELINE-1 DPOPT)  | 40020     | -76%     | 6263.4| +111% | 10|
| [V3] BANDWIDTH = 4 (PIPELINE-1 DPOPT)  | 35060     | -70%       | 7242.0    | +144% | 12|
| [V3] BANDWIDTH = 5 (PIPELINE-1 DPOPT)  | 32060     |-73%      | 8104.5     | +173% | 14|
| [V3] BANDWIDTH = 6 (PIPELINE-1 DPOPT)  | 30060     | -75%     | 8958.9 | +202% | 16|
| [V3] BANDWIDTH = 7 (PIPELINE-1 DPOPT)  | 28600     | -76%     | 9890.9 | +233% | 18|
| [V3] BANDWIDTH = 8 (PIPELINE-1 DPOPT)  | 27500     | -77%     | 10781.5 | +263% | 20|
| [V4] BANDWIDTH = 2 (PIPELINE-1 & UNROLLED LOOPS)  | 10040      | -91%      | 7093.3| +139% | 3|
| [V4] BANDWIDTH = 3 (PIPELINE-1 & UNROLLED LOOPS)  | 6710    | -94%     | 10457.2| +252% | 3|
| [V4] BANDWIDTH = 4 (PIPELINE-1 & UNROLLED LOOPS)  | **5040**    | **-95%**     |  13554.7| +356% | 3

At the two extremes, if we want only the lowest area, the original version without optimization is the smallest. If we only care about speed, the final version with pipelined unrolled loops run the fastest, albeit with much greater area.

If we want something down the middle, pipelined versions with fewer inputs work quite well, and does not use too much area.

### Application
Since I had some actual patient data on hand, I snipped out 2000 samples, and plot out the results of the module.

![](https://i.imgur.com/nqWmbt0.png)

The blue line indicate the original EEG signal, and the orange line is the output of the NEO module. We can see that the spikes in the blue line is somewhat difficult to makeout, especially if we had to look at multiple channels at the same time, or look at longer period of data. The spike on the orange line however is very apparent and distinguishing the spike should be much easier.

![](https://i.imgur.com/0EijQA5.png)

Here, we added an additional adaptive threshold calculated using pure software. It is clear that the threshold will easily weed out any unwanted noise, allowing us to quickly discern where the spikes are. Currently, the threshold caluculation is not done on the NEO module since it uses requires a buffer size of at least a few hundred samples to work properly, and thus would completely overwhelm the area size, making our comparisons between different architecture meaningless. 

However just with the NEO, we can see that using HLS, we can produce a design that is practical l in the real-time processing of EEG, and that the optimization directives allow us to speed up the design by a factor of nearly 20.
