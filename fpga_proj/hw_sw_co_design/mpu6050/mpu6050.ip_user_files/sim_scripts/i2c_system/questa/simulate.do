onbreak {quit -f}
onerror {quit -f}

vsim -t 1ps -pli "/opt/Xilinx/Vivado/2015.4/lib/lnx64.o/libxil_vsim.so" -lib xil_defaultlib i2c_system_opt

do {wave.do}

view wave
view structure
view signals

do {i2c_system.udo}

run -all

quit -force
