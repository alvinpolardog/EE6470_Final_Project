# EE6470_Final_Project
EE6470 Electronic System Level Design and Synthesis Final Project - Spike Detection via Nonlinear Energy Operator

吳哲廷 學號:110061590

## High Level Synthesis

How to run:
1. Login to NTHUCAD workstation or EE workstation to use HLS tools.
2. Set environment variable for Stratus tools (For EE workstation)
    ```
    source /usr/cadtool/user_setup/01-cadence_license_set.cshset
    source /usr/cadtool/user_setup/03-stratus.csh
    source /usr/cad/cadence/cic_setup/incisiv.cshrc
    setenv STRATUS_EXAMPLE /usr/cad/cadence/STRATUS/cur/share/stratus/collateral/examples
    ```

3. Copy the content of the HLS file to the workstation and navigate to the stratus directory
  ```
  cd stratus
  ```
4. Run HLS
  ```
  make sim_V_BASIC
  ```
  
   or
     
  ```
  make sim_V_DPA
  ```

6. Results are located in 

    '/bdw_work/modules/NEO/out.bin'



## RISCV-VP Simultion

How to run:
1. Log in to the Docker provided by EE6470
2. Download the RISCV-VP repo to $EE6470
```
$ cd $EE6470
$ git clone https://github.com/agra-uni-bremen/riscv-vp.git
$ cd riscv-vp
```

3. Build a local version of the SystemC and softfloat library:
```
$ cd vp/dependencies
$ ./build_systemc_233.sh
$ ././build_softfloat.sh
```

4. Add the 'basic-acc' directory to the platforms
```
$ cp -r {downloaded_directory}/hw/* $EE6470/riscv-vp/vp/src/platform
$ cd $EE6470/riscv-vp/vp/build
$ cmake ..
$ make install
```
5. Copy the riscv-vp testbench to /sw
```
$ cp -r {downloaded_directory}/sw/* $EE6470/riscv-vp/sw
```
6. Build Spike Detector
```
$ cd $EE6470
$ cd riscv-vp/sw
$ cd {name-of-sw-directory}
$ make
$ make sim
```
7. Resulting image is saved as 'output.bin'


