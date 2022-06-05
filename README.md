# EE6470_Final_Project
EE6470 Electronic System Level Design and Synthesis Final Project - Spike Detection via Nonlinear Energy Operator

吳哲廷 學號:110061590


How to run:
1. Login to NTHUCAD workstation or EE workstation to use HLS tools.
2. Set environment variable for Stratus tools (For EE workstation)
    ```
    source /usr/cadtool/user_setup/01-cadence_license_set.cshset
    source /usr/cadtool/user_setup/03-stratus.csh
    source /usr/cad/cadence/cic_setup/incisiv.cshrc
    setenv STRATUS_EXAMPLE /usr/cad/cadence/STRATUS/cur/share/stratus/collateral/examples
    ```

3. Navigate to the stratus directory
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

    '/bdw_work/modules/SobelFilter/DPA/stratus_hls.log' 

    and 

    '/bdw_work/sims/V_DPA/bdw_sim.log'



