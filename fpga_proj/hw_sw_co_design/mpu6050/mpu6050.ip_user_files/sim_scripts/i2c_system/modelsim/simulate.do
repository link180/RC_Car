onbreak {quit -f}
onerror {quit -f}

vsim -voptargs="+acc" -t 1ps -pli "/opt/Xilinx/Vivado/2015.4/lib/lnx64.o/libxil_vsim.so" -L secureip -L xil_defaultlib -lib xil_defaultlib xil_defaultlib.i2c_system

do {wave.do}

view wave
view structure
view signals

do {i2c_system.udo}

run -all

quit -force
