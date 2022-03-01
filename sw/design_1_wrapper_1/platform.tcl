# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/cwshea/git/vcu118-microblaze/sw/design_1_wrapper_1/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/cwshea/git/vcu118-microblaze/sw/design_1_wrapper_1/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {design_1_wrapper_1}\
-hw {/home/cwshea/git/vcu118-microblaze/hw/design_1_wrapper.xsa}\
-fsbl-target {psu_cortexa53_0} -out {/home/cwshea/git/vcu118-microblaze/sw}

platform write
domain create -name {standalone_microblaze_0} -display-name {standalone_microblaze_0} -os {standalone} -proc {microblaze_0} -runtime {cpp} -arch {32-bit} -support-app {hello_world}
platform generate -domains 
platform active {design_1_wrapper_1}
platform generate -quick
platform generate
platform clean
platform clean
