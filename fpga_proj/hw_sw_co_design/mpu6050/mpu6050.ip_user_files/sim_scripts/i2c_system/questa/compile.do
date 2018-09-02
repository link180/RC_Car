vlib work
vlib msim

vlib msim/xil_defaultlib

vmap xil_defaultlib msim/xil_defaultlib

vcom -work xil_defaultlib -64 \
"/home/sdr/zynq_fpga/hw_sw_co_design/mpu6050/mpu6050.srcs/sources_1/bd/i2c_system/ip/i2c_system_processing_system7_0_0/i2c_system_processing_system7_0_0_sim_netlist.vhdl" \
"../../../bd/i2c_system/hdl/i2c_system.vhd" \


