# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/cwshea/git/vcu118-microblaze/sw/vcu118-microblaze-base_system/_ide/scripts/debugger_vcu118-microblaze-base-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/cwshea/git/vcu118-microblaze/sw/vcu118-microblaze-base_system/_ide/scripts/debugger_vcu118-microblaze-base-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -filter {jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A3B9D8" && level==0 && jtag_device_ctx=="jsn-JTAG-SMT2NC-210308A3B9D8-14b31093-0"}
fpga -file /home/cwshea/git/vcu118-microblaze/sw/vcu118-microblaze-base/_ide/bitstream/design_1_wrapper.bit
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
loadhw -hw /home/cwshea/git/vcu118-microblaze/sw/design_1_wrapper_1/export/design_1_wrapper_1/hw/design_1_wrapper.xsa -regs
configparams mdm-detect-bscan-mask 2
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
rst -system
after 3000
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
dow /home/cwshea/git/vcu118-microblaze/sw/vcu118-microblaze-base/Debug/vcu118-microblaze-base.elf
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
con
