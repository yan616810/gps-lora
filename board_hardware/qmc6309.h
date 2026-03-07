#ifndef __QMC6309_H
#define __QMC6309_H

#define QMC6309_CHIPID_VALUE 0x90
//通过地磁计芯片，我焊接在pcb上并不平整，我如何软件校准？是不是我如果知道我本地地磁大小和磁倾角，当我将我的pcb朝向通过芯片输出的3轴磁大小可以合成唯一一个方向确定，大小确定的向量

/*memory map*/
#define QMC6309_CHIPID_REG                    0x00      //芯片ID寄存器(R-Only)
#define QMC6309_X_LSB_REG                     0x01      //X轴数据寄存器(R-Only)
#define QMC6309_X_MSB_REG                     0x02      //X轴数据寄存器(R-Only)
#define QMC6309_Y_LSB_REG                     0x03      //Y轴数据寄存器(R-Only)
#define QMC6309_Y_MSB_REG                     0x04      //Y轴数据寄存器(R-Only)
#define QMC6309_Z_LSB_REG                     0x05      //Z轴数据寄存器(R-Only)
#define QMC6309_Z_MSB_REG                     0x06      //Z轴数据寄存器(R-Only)
#define QMC6309_STATUS_REG                    0x09      //状态寄存器(R-Only)
#define QMC6309_CTRL_1_REG                    0x0A      //控制寄存器(R/W)
#define QMC6309_CTRL_2_REG                    0x0B      //控制寄存器(R/W)
#define QMC6309_CTRL_3_REG                    0x0E      //控制寄存(R/W)
#define QMC6309_X_Self_Test_REG               0x13      //X轴自检寄存器(R-Only)
#define QMC6309_Y_Self_Test_REG               0x14      //Y轴自检寄存器(R-Only)
#define QMC6309_Z_Self_Test_REG               0x15      //Z轴自检寄存器(R-Only)


#endif /* __QMC6309_H */