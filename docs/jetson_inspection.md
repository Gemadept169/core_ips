# Jetson AGX Orin Series
- The Jetson AGX Orin series modules have two 3-channel INA3221 power monitors at I2C addresses 0x40 and 0x41. The sysfs nodes to read for rail names, voltage, current, and instantaneous and average current limit are at:
```
/sys/bus/i2c/drivers/ina3221/1-0040/hwmon/hwmon<x>
/sys/bus/i2c/drivers/ina3221/1-0041/hwmon/hwmon<y>
(Where <x> and <y> are dynamic hwmon indexes)
```

- The rail names for I2C address 0x40 are:

| **Channel** | **Rail Name** | **Description**                 |
| ----------- | ------------- | ------------------------------- |
| Channel 1   | `VDD_GPU_SOC` | GPU and SOC Combined power rail |
| Channel 2   | `VDD_CPU_CV`  | CPU and CV Combined power rail  |
| Channel 3   | `VIN_SYS_5V0` | System 5V power rail            |

- The rail names for I2C address 0x41 are:

| **Channel** | **Rail Name**     | **Description**                   |
| ----------- | ----------------- | --------------------------------- |
| Channel 1   | `NC`              | No connection                     |
| Channel 2   | `VDDQ_VDD2_1V8AO` | DDR and 1V8AO combined power rail |
| Channel 3   | `NC`              | No connection                     |

# Examples
- To read INA3221 at 0x40, the channel-1 rail name, enter the command:
```
cat /sys/bus/i2c/drivers/ina3221/1-0040/hwmon/hwmon<x>/in1_label
```

- To read channel-1 voltage and current, enter the commands:
```
cat /sys/bus/i2c/drivers/ina3221/1-0040/hwmon/hwmon<x>/in1_input
cat /sys/bus/i2c/drivers/ina3221/1-0040/hwmon/hwmon<x>/curr1_input
```

- To read the channel-1 instantaneous current limit, enter the command:
```
cat /sys/bus/i2c/drivers/ina3221/1-0040/hwmon/hwmon<x>/curr1_crit
```

- To set the channel-1 instantaneous current limit, enter the command:
```
echo <current> > /sys/bus/i2c/drivers/ina3221/1-0040/hwmon/hwmon<x>/curr1_crit
```

- To read the channel-1 average current limit, enter the command:
```
cat /sys/bus/i2c/drivers/ina3221/1-0040/hwmon/hwmon<x>/curr1_max
```

- To set the channel-1 average current limit, enter the command:
```
echo <current> > /sys/bus/i2c/drivers/ina3221/1-0040/hwmon/hwmon<x>/curr1_max
```
Where <current> is the current limit to be set for the rail, in milliamperes.