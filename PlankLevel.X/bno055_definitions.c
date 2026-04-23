// definitions for the BNO055 as an IMU
#define IMU 0x28 // default address

// gyro data registers
#define EUL_DATA_X_LSB 0x1A
#define EUL_DATA_X_MSB 0x1B
#define EUL_DATA_Y_LSB 0x1C
#define EUL_DATA_Y_MSB 0x1D
#define EUL_DATA_Z_LSB 0x1E
#define EUL_DATA_Z_MSB 0x1F

// config registers
#define UNIT_SEL 0x3B
#define OPR_MODE 0x3D
#define PWR_MODE 0x3E

// gyro config registers
#define GYR_CONFIG_0 0x0A
#define GYR_CONFIG_1 0x0B
#define GYR_SLEEP_CONFIG 0x0D

// gyro config offset registers
#define GYR_OFFSET_X_LSB 0x61
#define GYR_OFFSET_X_MSB 0x62
#define GYR_OFFSET_Y_LSB 0x63
#define GYR_OFFSET_Y_MSB 0x64
#define GYR_OFFSET_Z_LSB 0x65
#define GYR_OFFSET_Z_MSB 0x66

// status registers
#define SYS_CLK_STATUS 0x38
#define SYS_STATUS 0x39
#define CALIB_STAT 0x35

// interrupt registers
#define INT_MASK 0x0F
#define INT_EN 0x10
#define INT_STA 0x37
