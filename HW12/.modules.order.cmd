cmd_/home/odiachuk/Desktop/LunuxProCamp/HW12/modules.order := {   echo /home/odiachuk/Desktop/LunuxProCamp/HW12/platform_test.ko;   echo /home/odiachuk/Desktop/LunuxProCamp/HW12/platform_cdev.ko; :; } | awk '!x[$$0]++' - > /home/odiachuk/Desktop/LunuxProCamp/HW12/modules.order
