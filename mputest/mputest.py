
import usb.core
import time

class mputest:

    def __init__(self):
        self.TOGGLE_LED1 = 1
        self.TOGGLE_LED2 = 2
        self.READ_SW1 = 3
        self.MPU_WRITE_REG = 4
        self.MPU_READ_REG = 5
        self.MPU_WRITE_REGS = 6
        self.MPU_READ_REGS = 7
        self.TOGGLE_LED3 = 8
        self.READ_SW2 = 9
        self.READ_SW3 = 10
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

# MPU-9250 Register Map for Gyroscope and Accelerometer
        self.MPU_SELF_TEST_X_GYRO = 0x00
        self.MPU_SELF_TEST_Y_GYRO = 0x01
        self.MPU_SELF_TEST_Z_GYRO = 0x02
        self.MPU_SELF_TEST_X_ACCEL = 0x0D
        self.MPU_SELF_TEST_Y_ACCEL = 0x0E
        self.MPU_SELF_TEST_Z_ACCEL = 0x0F
        self.MPU_XG_OFFSET_H = 0x13
        self.MPU_XG_OFFSET_L = 0x14
        self.MPU_YG_OFFSET_H = 0x15
        self.MPU_YG_OFFSET_L = 0x16
        self.MPU_ZG_OFFSET_H = 0x17
        self.MPU_ZG_OFFSET_L = 0x18
        self.MPU_SMPLRT_DIV = 0x19
        self.MPU_CONFIG = 0x1A
        self.MPU_GYRO_CONFIG = 0x1B
        self.MPU_ACCEL_CONFIG = 0x1C
        self.MPU_ACCEL_CONFIG2 = 0x1D
        self.MPU_LP_ACCEL_ODR = 0x1E
        self.MPU_WOM_THR = 0x1F
        self.MPU_FIFO_EN = 0x23
        self.MPU_I2C_MST_CTRL = 0x24
        self.MPU_I2C_SLV0_ADDR = 0x25
        self.MPU_I2C_SLV0_REG = 0x26
        self.MPU_I2C_SLV0_CTRL = 0x27
        self.MPU_I2C_SLV1_ADDR = 0x28
        self.MPU_I2C_SLV1_REG = 0x29
        self.MPU_I2C_SLV1_CTRL = 0x2A
        self.MPU_I2C_SLV2_ADDR = 0x2B
        self.MPU_I2C_SLV2_REG = 0x2C
        self.MPU_I2C_SLV2_CTRL = 0x2D
        self.MPU_I2C_SLV3_ADDR = 0x2E
        self.MPU_I2C_SLV3_REG = 0x2F
        self.MPU_I2C_SLV3_CTRL = 0x30
        self.MPU_I2C_SLV4_ADDR = 0x31
        self.MPU_I2C_SLV4_REG = 0x32
        self.MPU_I2C_SLV4_DO = 0x33
        self.MPU_I2C_SLV4_CTRL = 0x34
        self.MPU_I2C_SLV4_DI = 0x35
        self.MPU_I2C_MST_STATUS = 0x36
        self.MPU_INT_PIN_CFG = 0x37
        self.MPU_INT_ENABLE = 0x38
        self.MPU_INT_STATUS = 0x3A
        self.MPU_ACCEL_XOUT_H = 0x3B
        self.MPU_ACCEL_XOUT_L = 0x3C
        self.MPU_ACCEL_YOUT_H = 0x3D
        self.MPU_ACCEL_YOUT_L = 0x3E
        self.MPU_ACCEL_ZOUT_H = 0x3F
        self.MPU_ACCEL_ZOUT_L = 0x40
        self.MPU_TEMP_OUT_H = 0x41
        self.MPU_TEMP_OUT_L = 0x42
        self.MPU_GYRO_XOUT_H = 0x43
        self.MPU_GYRO_XOUT_L = 0x44
        self.MPU_GYRO_YOUT_H = 0x45
        self.MPU_GYRO_YOUT_L = 0x46
        self.MPU_GYRO_ZOUT_H = 0x47
        self.MPU_GYRO_ZOUT_L = 0x48
        self.MPU_EXT_SENS_DATA_00 = 0x49
        self.MPU_EXT_SENS_DATA_01 = 0x4A
        self.MPU_EXT_SENS_DATA_02 = 0x4B
        self.MPU_EXT_SENS_DATA_03 = 0x4C
        self.MPU_EXT_SENS_DATA_04 = 0x4D
        self.MPU_EXT_SENS_DATA_05 = 0x4E
        self.MPU_EXT_SENS_DATA_06 = 0x4F
        self.MPU_EXT_SENS_DATA_07 = 0x50
        self.MPU_EXT_SENS_DATA_08 = 0x51
        self.MPU_EXT_SENS_DATA_09 = 0x52
        self.MPU_EXT_SENS_DATA_10 = 0x53
        self.MPU_EXT_SENS_DATA_11 = 0x54
        self.MPU_EXT_SENS_DATA_12 = 0x55
        self.MPU_EXT_SENS_DATA_13 = 0x56
        self.MPU_EXT_SENS_DATA_14 = 0x57
        self.MPU_EXT_SENS_DATA_15 = 0x58
        self.MPU_EXT_SENS_DATA_16 = 0x59
        self.MPU_EXT_SENS_DATA_17 = 0x5A
        self.MPU_EXT_SENS_DATA_18 = 0x5B
        self.MPU_EXT_SENS_DATA_19 = 0x5C
        self.MPU_EXT_SENS_DATA_20 = 0x5D
        self.MPU_EXT_SENS_DATA_21 = 0x5E
        self.MPU_EXT_SENS_DATA_22 = 0x5F
        self.MPU_EXT_SENS_DATA_23 = 0x60
        self.MPU_I2C_SLV0_DO = 0x63
        self.MPU_I2C_SLV1_DO = 0x64
        self.MPU_I2C_SLV2_DO = 0x65
        self.MPU_I2C_SLV3_DO = 0x66
        self.MPU_I2C_MST_DELAY_CTRL = 0x67
        self.MPU_SIGNAL_PATH_RESET = 0x68
        self.MPU_MOT_DETECT_CTRL = 0x69
        self.MPU_USER_CTRL = 0x6A
        self.MPU_PWR_MGMT_1 = 0x6B
        self.MPU_PWR_MGMT_2 = 0x6C
        self.MPU_FIFO_COUNTH = 0x72
        self.MPU_FIFO_COUNTL = 0x73
        self.MPU_FIFO_R_W = 0x74
        self.MPU_WHO_AM_I = 0x75
        self.MPU_XA_OFFSET_H = 0x77
        self.MPU_XA_OFFSET_L = 0x78
        self.MPU_YA_OFFSET_H = 0x7A
        self.MPU_YA_OFFSET_L = 0x7B
        self.MPU_ZA_OFFSET_H = 0x7D
        self.MPU_ZA_OFFSET_L = 0x7E

# The MPU-9250's magnetometer is on a separate die within the package that 
# is made by a different manufacturer, Asahi Kasei Microdevices (AKM), part 
# number AK8963.  The MPU-9250 communicates with the AK8963 via I2C as a
# slave device at I2C address 0x0C.
        self.MAG_I2C_ADDR = 0x0C

# MPU-9250 Magnetometer Register Map
        self.MAG_WIA = 0x00
        self.MAG_INFO = 0x01
        self.MAG_ST1 = 0x02
        self.MAG_HXL = 0x03
        self.MAG_HXH = 0x04
        self.MAG_HYL = 0x05
        self.MAG_HYH = 0x06
        self.MAG_HZL = 0x07
        self.MAG_HZH = 0x08
        self.MAG_ST2 = 0x09
        self.MAG_CNTL1 = 0x0A
        self.MAG_CNTL2 = 0x0B
        self.MAG_ASTC = 0x0C
        self.MAG_ASAX = 0x10
        self.MAG_ASAY = 0x11
        self.MAG_ASAZ = 0x12

        self.accel_mults = [1./16384., 1./8192., 1./4096., 1./2048.]
        self.accel_mult = self.accel_mults[1]
        self.gyro_mults = [1./131., 1./65.5, 1./32.8, 1./16.4]
        self.gyro_mult = self.gyro_mults[3]
        self.mag_x_mult = 0.15
        self.mag_y_mult = 0.15
        self.mag_z_mult = 0.15

        self.mag_delay = 0.001

    def close(self):
        self.dev = None

    def toggle_led1(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_LED1)
        except usb.core.USBError:
            print "Could not send TOGGLE_LED1 vendor request."

    def toggle_led2(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_LED2)
        except usb.core.USBError:
            print "Could not send TOGGLE_LED2 vendor request."

    def toggle_led3(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_LED3)
        except usb.core.USBError:
            print "Could not send TOGGLE_LED3 vendor request."

    def read_sw1(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_SW1, 0, 0, 1)
        except usb.core.USBError:
            print "Could not send READ_SW1 vendor request."
        else:
            return int(ret[0])

    def read_sw2(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_SW2, 0, 0, 1)
        except usb.core.USBError:
            print "Could not send READ_SW2 vendor request."
        else:
            return int(ret[0])

    def read_sw3(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_SW3, 0, 0, 1)
        except usb.core.USBError:
            print "Could not send READ_SW3 vendor request."
        else:
            return int(ret[0])

    def mpu_writeReg(self, address, value):
        try:
            self.dev.ctrl_transfer(0x40, self.MPU_WRITE_REG, address, value)
        except usb.core.USBError:
            print "Could not send MPU_WRITE_REG vendor request."

    def mpu_readReg(self, address):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.MPU_READ_REG, address, 0, 1)
        except usb.core.USBError:
            print "Could not send MPU_READ_REG vendor request."
        else:
            return int(ret[0])

    def mpu_writeRegs(self, address, values):
        try:
            self.dev.ctrl_transfer(0x40, self.MPU_WRITE_REGS, address, 0, values)
        except usb.core.USBError:
            print "Could not send MPU_WRITE_REGS vendor request."

    def mpu_readRegs(self, address, num_bytes):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.MPU_READ_REGS, address, 0, num_bytes)
        except usb.core.USBError:
            print "Could not send MPU_READ_REGS vendor request."
        else:
            return [int(val) for val in ret]

    def mpu_init(self):
        # Reset the MPU-9250.
        self.mpu_writeReg(self.MPU_PWR_MGMT_1, 0x80)
        # Use DLPF, set gyro bandwidth to 184 Hz and temp bandwidth to 188 Hz.
        self.mpu_writeReg(self.MPU_CONFIG, 0x01)
        # Set gyro range to +/-2000 dps.
        self.mpu_writeReg(self.MPU_GYRO_CONFIG, 0x18)
        # Set accel range to +/-4 g.
        self.mpu_writeReg(self.MPU_ACCEL_CONFIG, 0x08)
        # Set accel data rate, enable accel LPF, set bandwith to 184 Hz.
        self.mpu_writeReg(self.MPU_ACCEL_CONFIG2, 0x09)
        # Configure INT pin to latch and clear on any read
        self.mpu_writeReg(self.MPU_INT_PIN_CFG, 0x30)
        # Set I2C master mode, reset I2C slave module, and put serial interface
        # in SPI mode.
        self.mpu_writeReg(self.MPU_USER_CTRL, 0x30)
        # Confiugre I2C slave interface for a 400-kHz clock.
        self.mpu_writeReg(self.MPU_I2C_MST_CTRL, 0x0D)
        # Set I2C Slave 0 address to AK8963's I2C address.
        self.mpu_writeReg(self.MPU_I2C_SLV0_ADDR, self.MAG_I2C_ADDR)

        # Reset the AK8963.
        self.mpu_writeReg(self.MPU_I2C_SLV0_REG, self.MAG_CNTL2)
        self.mpu_writeReg(self.MPU_I2C_SLV0_DO, 0x01)
        self.mpu_writeReg(self.MPU_I2C_SLV0_CTRL, 0x81)
        time.sleep(self.mag_delay)

        # Put AK8963 in fuse ROM access mode.
        self.mpu_writeReg(self.MPU_I2C_SLV0_REG, self.MAG_CNTL1)
        self.mpu_writeReg(self.MPU_I2C_SLV0_DO, 0x0F)
        self.mpu_writeReg(self.MPU_I2C_SLV0_CTRL, 0x81)
        time.sleep(self.mag_delay)

        # Read ASA values from AK8963.
        self.mpu_writeReg(self.MPU_I2C_SLV0_ADDR, self.MAG_I2C_ADDR|0x80)
        self.mpu_writeReg(self.MPU_I2C_SLV0_REG, self.MAG_ASAX)
        self.mpu_writeReg(self.MPU_I2C_SLV0_CTRL, 0x83)
        time.sleep(self.mag_delay)
        values = self.mpu_readRegs(self.MPU_EXT_SENS_DATA_00, 3)
        self.mag_x_mult = 0.15*(float(values[0] - 128)/256. + 1.)
        self.mag_y_mult = 0.15*(float(values[1] - 128)/256. + 1.)
        self.mag_z_mult = 0.15*(float(values[2] - 128)/256. + 1.)

        # Put AK8963 into power-down mode.
        self.mpu_writeReg(self.MPU_I2C_SLV0_ADDR, self.MAG_I2C_ADDR)
        self.mpu_writeReg(self.MPU_I2C_SLV0_REG, self.MAG_CNTL1)
        self.mpu_writeReg(self.MPU_I2C_SLV0_DO, 0x00)
        self.mpu_writeReg(self.MPU_I2C_SLV0_CTRL, 0x81)
        time.sleep(self.mag_delay)

        # Configure AK8963 for continuous 16-bit measurement mode at 100 Hz.
        self.mpu_writeReg(self.MPU_I2C_SLV0_REG, self.MAG_CNTL1)
        self.mpu_writeReg(self.MPU_I2C_SLV0_DO, 0x16)
        self.mpu_writeReg(self.MPU_I2C_SLV0_CTRL, 0x81)
        time.sleep(self.mag_delay)

        self.mpu_set_accel_scale(2.)
        self.mpu_set_gyro_scale(250.)

    def mpu_set_accel_scale(self, scale):
        if scale<=2.:
            # Set accel range to +/-2 g.
            self.mpu_writeReg(self.MPU_ACCEL_CONFIG, 0x00)
            self.accel_mult = self.accel_mults[0]
        elif scale<=4.:
            # Set accel range to +/-4 g.
            self.mpu_writeReg(self.MPU_ACCEL_CONFIG, 0x08)
            self.accel_mult = self.accel_mults[1]
        elif scale<=8.:
            # Set accel range to +/-8 g.
            self.mpu_writeReg(self.MPU_ACCEL_CONFIG, 0x10)
            self.accel_mult = sefl.accel_mults[2]
        else:
            # Set accel range to +/-16 g.
            self.mpu_writeReg(self.MPU_ACCEL_CONFIG, 0x18)
            self.accel_mult = self.accel_mults[3]

    def mpu_get_accel_scale(self):
        scale = self.mpu_readReg(self.MPU_ACCEL_CONFIG)
        scale = scale>>3
        self.accel_mult = self.accel_mults[scale]
        return 2.*(1<<scale)

    def mpu_set_gyro_scale(self, scale):
        if scale<=250.:
            # Set gyro range to +/-250 dps.
            self.mpu_writeReg(self.MPU_GYRO_CONFIG, 0x00)
            self.gyro_mult = self.gyro_mults[0]
        elif scale<=500.:
            # Set gyro range to +/-500 dps.
            self.mpu_writeReg(self.MPU_GYRO_CONFIG, 0x08)
            self.gyro_mult = self.gyro_mults[1]
        elif scale<=1000.:
            # Set gyro range to +/-1000 dps.
            self.mpu_writeReg(self.MPU_GYRO_CONFIG, 0x10)
            self.gyro_mult = self.gyro_mults[2]
        else:
            # Set gyro range to +/-2000 dps.
            self.mpu_writeReg(self.MPU_GYRO_CONFIG, 0x18)
            self.gyro_mult = self.gyro_mults[3]

    def mpu_get_gyro_scale(self):
        scale = self.mpu_readReg(self.MPU_GYRO_CONFIG)
        scale = scale>>3
        self.gyro_mult = self.gyro_mults[scale]
        return 250.*(1<<scale)

    def mpu_whoami(self):
        return self.mpu_readReg(self.MPU_WHO_AM_I)

    def mpu_read_accel(self):
        values = self.mpu_readRegs(self.MPU_ACCEL_XOUT_H, 6)

        x = 256*values[0] + values[1]
        y = 256*values[2] + values[3]
        z = 256*values[4] + values[5]

        x = x - 65536 if x>32767 else x
        y = y - 65536 if y>32767 else y
        z = z - 65536 if z>32767 else z

        return [float(x)*self.accel_mult, float(y)*self.accel_mult, float(z)*self.accel_mult]

    def mpu_read_gyro(self):
        values = self.mpu_readRegs(self.MPU_GYRO_XOUT_H, 6)

        x = 256*values[0] + values[1]
        y = 256*values[2] + values[3]
        z = 256*values[4] + values[5]

        x = x - 65536 if x>32767 else x
        y = y - 65536 if y>32767 else y
        z = z - 65536 if z>32767 else z

        return [float(x)*self.gyro_mult, float(y)*self.gyro_mult, float(z)*self.gyro_mult]

    def mpu_read_temp(self):
        values = self.mpu_readRegs(self.MPU_TEMP_OUT_H, 2)
        value = 256*values[0] + values[1]
        value = value - 65536 if value>32767 else value
        return value

    def mpu_mag_whoami(self):
        self.mpu_writeReg(self.MPU_I2C_SLV0_ADDR, self.MAG_I2C_ADDR|0x80)
        self.mpu_writeReg(self.MPU_I2C_SLV0_REG, self.MAG_WIA)
        self.mpu_writeReg(self.MPU_I2C_SLV0_CTRL, 0x81)
        time.sleep(self.mag_delay)
        return self.mpu_readReg(self.MPU_EXT_SENS_DATA_00)

    def mpu_read_mag(self):
        self.mpu_writeReg(self.MPU_I2C_SLV0_ADDR, self.MAG_I2C_ADDR|0x80)
        self.mpu_writeReg(self.MPU_I2C_SLV0_REG, self.MAG_HXL)
        self.mpu_writeReg(self.MPU_I2C_SLV0_CTRL, 0x87)
        time.sleep(self.mag_delay)
        values = self.mpu_readRegs(self.MPU_EXT_SENS_DATA_00, 6)

        x = values[0] + 256*values[1]
        y = values[2] + 256*values[3]
        z = values[4] + 256*values[5]

        x = x - 65536 if x>32767 else x
        y = y - 65536 if y>32767 else y
        z = z - 65536 if z>32767 else z

        return [float(x)*self.mag_x_mult, float(y)*self.mag_y_mult, float(z)*self.mag_z_mult]

