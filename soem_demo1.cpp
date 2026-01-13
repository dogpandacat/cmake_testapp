//
// Created by AndyCui on 2023/10/12.
//

#include <iostream>
#include "osal.h"
#include "ethercat.h"

#include <cstdio>
#include <cstring>


#define EC_TIMEOUTMON 500

char IOmap[4096];
OSAL_THREAD_HANDLE thread1;
int expectedWKC;
boolean needlf;
volatile int wkc;
volatile int rtcnt;
boolean inOP;
uint8 currentgroup = 0;

/* most basic RT thread for process data, just does IO transfer */
void CALLBACK RTthread(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1,  DWORD_PTR dw2)
{
IOmap[0]++;

ec_send_processdata();
wkc = ec_receive_processdata(EC_TIMEOUTRET);
rtcnt++;
/* do RT control stuff here */
}

int EL7031setup(uint16 slave)
{
    int retval;
    uint16 u16val;

    // map velocity
    uint16 map_1c12[4] = {0x0003, 0x1601, 0x1602, 0x1604};
    uint16 map_1c13[3] = {0x0002, 0x1a01, 0x1a03};

    retval = 0;

    // Set PDO mapping using Complete Access
    // Strange, writing CA works, reading CA doesn't
    // This is a protocol error of the slave.

    /**
     * https://manual.zlg.cn/web/#/211/7889
    EcatState类型是表示总线状态的枚举数据类型，使用的类型值有如下几种：
　　EcatStateI：Init初始化状态。在此状态下，主站和从站之间没有应用层上的通讯，主站 可以访问从站DL相关状态寄存器信息。
　　EcatStateP：Pre-Operational预操作状态。在此状态下，如果从站支持邮箱通信，主站和 从站可以使用邮箱和相关协议进行应用层次的初始化和参数配置，此状态下不能进行过程数 据通信。
　　EcatStateB：Bootstrap状态。在此状态下，从站可以通过FOE协议接收固件进行升级。File Over EtherCAT，这可以升级从站的固件，不错！
　　EcatStateS：Safe-Operational 安全操作状态。在此状态下，可以进行过程数据通信，从站可以进行数据的输入，
                                             但是不允许数据输出，数据输出处于“安全”状态。

　　EcatStateO：Operational运行状态。在此状态下，从站可以进行数据的输入输出操作。
     *
     *
     *
     * 1、EtherCAT通信模式
     *
     * EtherCAT通信是以主从通讯模式进行的，其中主站控制着EtherCAT系统通信。在实际自动化控制应用中，
     * 通信数据一般可分为：时间关键的和非时间关键。在EtherCAT中利用周期性过程数据通信（比如每100us，或者每500us）来进行时间关键数据通信，
     * 而采用非周期性邮箱通信（mailbox）来实现非时间关键数据通信。
     *
     * （1）周期性过程数据通信

周期性过程数据通信通常使用现场总线内存管理单元（FMMU）进行逻辑寻址，主站可以通过逻辑读写命令来操作从站。周期性过程数据通信使用两个存储同步管理单元（SM）来保证数据交换的一致性和安全性，通讯模式采用缓存模式。在缓存模式下使用三个相同大小的缓冲区，由SM统一管理，缓存模式的运行原理如图所示。

（2）非周期性邮箱通信

邮箱数据通信模式只使用一个缓冲区，为保证数据不丢失，数据交换采用握手机制，即在一端完成对缓冲区数据操作后，另一端才能操作缓冲区数据。通过这种轮流方式进行读写操作，来实现邮箱数据交换。
     *
     * 1、PDO Assign
     * PDO Assign是指PDO分配。在EtherCAT中，有两个CoE对象0x1C12“Rx PDO Assign”和0x1C13“Tx PDO Assign”，
     * 它们分别用于数据输入（RxPDO）和数据输出（TxPDO）的分配。
     * 具备应用控制功能的EtherCAT从站应该支持PDO映射和SM-PDO-Assign对象的读取
     *
     *
     * 2、如何配置Rx PDO Assign和Tx PDO Assign？
     *
     *
     * 3、什么是同步管理器通道？
     *
     * 同步管理器通道是EtherCAT从站中用于数据交换的通道。它负责在EtherCAT主站和从站之间同步数据传输。
     * 例如，通过分布式时钟程序，可以同步操作多个EtherCAT从站和/或EtherCAT主站
     * EtherCAT协议适用于自动化技术、测试和测量以及许多其他应用中的硬实时和软实时要求。
     * 在开发EtherCAT时，主要关注短周期时间（≤ 100 µs）、低抖动以实现精确同步（≤ 1 µs）等特点
     *
     * 3、什么是PDO映射？
     *
     * PDO映射定义了哪些应用程序对象在PDO中传输。
     * 使用EtherCAT从站时，必须映射新的PDO（过程数据对象）配置，必须修改.xml文件。
     * EtherCAT过程数据配置允许非常灵活的PDO描述。当使用MDP对象时，输入（0x6000 – 0x6FFF）和
     * 输出（0x7000 – 0x7FFF）被映射到PDO Mapping对象（0x1600 – 0x16FF用于RxPDO Mapping和0x1A00 – 0x1BFF
     * 用于TxPDO Mapping），并分配给同步管理器通道
     *
     * 4、什么是分布式时钟程序？
     *
     * 分布式时钟技术是EtherCAT系统中一种能够实现所有EtherCAT设备（主站和从站）中的本地时钟同步运行的技术。
     * 如果一个EtherCAT从站支持分布式时钟（DC），
     * 那么它的ESC（EtherCAT从站控制器）包含一个硬件时钟（通常为64位，较少情况下为32位），分辨率为1位=1纳秒1。
     *
     * 在EtherCAT术语中，“分布式时钟”指的是一个逻辑上的分布式时钟网络。
     * 通过使用分布式时钟，EtherCAT实时以太网协议能够在所有本地总线设备中同步时间，误差范围非常小2。
     *
     * 5、什么是硬件时钟？
     *
     * 硬件时钟是一种物理时钟，能够在系统关闭电源的情况下保持时间。它可以依靠化学或机械工作，或通过自己的独立电源来实现。
     * 硬件时钟通常是主板上的一个组件，它始终独立于操作系统的状态运行，即使计算机关闭也是如此。
     * 例如，在Linux中，硬件时钟指主机板上的时钟设备，也就是通常可在BIOS画面设定的时钟。
     *
     * 6、如何配置分布式时钟
     *
     * 在EtherCAT系统中，分布式时钟系统在EtherCAT启动时，在从PREOP状态转换到OP状态时同步。
     * 在DC-capable从站中，OP状态下的正确同步通常很重要。否则，从站会自动返回到PREOP状态。
     * 对于同步，指定了一个参考时钟。通常情况下，在主站之后的第一个DC设备管理参考时间。
     * EtherCAT主站使用其时钟来初始化参考时钟。但随后，EtherCAT主站也根据这个参考时间进行同步。
     *
     * 7、什么是PREOP和OP状态？
     *
     * 在EtherCAT系统中，PREOP（Pre-Operational）和OP（Operational）是两种状态。
     * 在OP状态下，从站将主站的输出数据复制到其输出端。此时，可以进行过程数据和邮箱通信。
     * 在EtherCAT主站将EtherCAT从站从Safe-Op切换到Op之前，它必须传输有效的输出数据。
     *
     * 8、什么是硬实时和软实时？
     * 硬实时和软实时是实时操作系统的两种类型。硬实时要求在规定的时间内必须完成操作，这是在操作系统设计时保证的。
     * 而软实时则只能提供统计意义上的实时，例如有的应用要求系统在95%的情况下都会确保在规定的时间内完成某个动作。
     *
     * 汽车的安全气囊触发机制就是一个硬实时功能的例子，说白了就是传感器探测到汽车碰撞后产生中断，RTOS必须立刻响应中断，
     * 在中断函数里面把气囊弹出来。
     *
     * 反导弹系统也要求硬实时，它由一系列硬实时任务组成，必须首先探测所有来袭导弹，正确定位反导炮，然后在导弹来袭之前将其摧毁。
     *
     * 9、EtherCAT通信中的CompleteAccess是什么意思？
     *
     * Complete Access 是指支持上传或下载完整的 CanOpen 对象。属于主索引的整个数据区域及其所有子索引都将被读取或写入。
     * 因为Canopen协议的寄存器有索引、子索引的说法，CA就是不指定子索引，一次性读/写某个索引下的所有子索引。
     *
     * 假设某个寄存器地址为0x3333，它有0x00, 0x01, 0x02, 0x03一共4个子索引，如果要一次性读取这4个子索引的值，就用CA模式。
     *
     * 10、脉冲当量
     *
     * 从脉冲形伺服的角度来解释，脉冲当量是指当PLC或者工控机输出一个定位控制脉冲时，如果走直线运动指移动的距离；
     * 对圆周运动来说，是指其转动的角度。简单来说，就是发一个脉冲，电机能走多少距离，也就是电机的最小精度。
     *
     * 如果从编码器反馈电机运行的角度来解释的话，那 脉冲当量 = 伺服电机编码器分辨率 / 360°，
     * 就是电机每转1°，编码器会产生多少个脉冲，那反过来根据脉冲当量，就可以计算出
     * 电机转过的角度了，这个脉冲当量，其实就是绝对值编码器中存储的值，我们可以通过SPI协议从绝对值编码器芯片中读取。
     *
     * 11、什么是机械零点？为什么需要机械零点？
     *
     * 机械零点是在机器的空间坐标里设置的一个零点，那里各轴坐标值为0。
     * 它是机械的零点，是永远不会变的，所以它是绝对坐标的零点。为安全起见，数控类机器都安装有限位开关，也就是零点了。
     *
     */
    retval += ec_SDOwrite(slave, 0x1c12, 0x00, TRUE, sizeof(map_1c12), &map_1c12, EC_TIMEOUTSAFE);
    retval += ec_SDOwrite(slave, 0x1c13, 0x00, TRUE, sizeof(map_1c13), &map_1c13, EC_TIMEOUTSAFE);

    // bug in EL7031 old firmware, CompleteAccess for reading is not supported even if the slave says it is.
    ec_slave[slave].CoEdetails &= ~ECT_COEDET_SDOCA;

    // set some motor parameters, just as example
    u16val = 1200; // max motor current in mA
//    retval += ec_SDOwrite(slave, 0x8010, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTSAFE);
    u16val = 150; // motor coil resistance in 0.01ohm
//    retval += ec_SDOwrite(slave, 0x8010, 0x04, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTSAFE);

    // set other nescessary parameters as needed
    // .....

    while(EcatError) printf("%s", ec_elist2string());

    printf("EL7031 slave %d set, retval = %d\n", slave, retval);
    return 1;
}

int AEPsetup(uint16 slave)
{
    int retval;
    uint8 u8val;
    uint16 u16val;

    retval = 0;

    u8val = 0;
    retval += ec_SDOwrite(slave, 0x1c12, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    u16val = 0x1603;
    retval += ec_SDOwrite(slave, 0x1c12, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);
    u8val = 1;
    retval += ec_SDOwrite(slave, 0x1c12, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    u8val = 0;
    retval += ec_SDOwrite(slave, 0x1c13, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    u16val = 0x1a03;
    retval += ec_SDOwrite(slave, 0x1c13, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);
    u8val = 1;
    retval += ec_SDOwrite(slave, 0x1c13, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    u8val = 8;
    retval += ec_SDOwrite(slave, 0x6060, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    // set some motor parameters, just as example
    u16val = 1200; // max motor current in mA
//    retval += ec_SDOwrite(slave, 0x8010, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTSAFE);
    u16val = 150; // motor coil resistance in 0.01ohm
//    retval += ec_SDOwrite(slave, 0x8010, 0x04, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTSAFE);

    // set other nescessary parameters as needed
    // .....

    while(EcatError) printf("%s", ec_elist2string());

    printf("AEP slave %d set, retval = %d\n", slave, retval);
    return 1;
}

void simpletest(char *ifname)
{
    int i, j, oloop, iloop, wkc_count, chk, slc;
    UINT mmResult;

    needlf = FALSE;
    inOP = FALSE;

    printf("Starting simple test\n");

    /* initialise SOEM, bind socket to ifname */
    if (ec_init(ifname))
    {
        printf("ec_init on %s succeeded.\n",ifname);
        /* find and auto-config slaves */

        //ec_config_init函数让所有从站进入PRE_OP状态
        if ( ec_config_init(FALSE) > 0 )
        {
            printf("%d slaves found and configured.\n",ec_slavecount);

            if((ec_slavecount > 1))
            {
                for(slc = 1; slc <= ec_slavecount; slc++)
                {
                    // beckhoff EL7031, using ec_slave[].name is not very reliable
                    if((ec_slave[slc].eep_man == 0x00000002) && (ec_slave[slc].eep_id == 0x1b773052))
                    {
                        printf("Found %s at position %d\n", ec_slave[slc].name, slc);  //打印从站名称
                        // link slave specific setup to preop->safeop hook
                        ec_slave[slc].PO2SOconfig = &EL7031setup;
                    }
                    // Copley Controls EAP, using ec_slave[].name is not very reliable
                    if((ec_slave[slc].eep_man == 0x000000ab) && (ec_slave[slc].eep_id == 0x00000380))
                    {
                        printf("Found %s at position %d\n", ec_slave[slc].name, slc);
                        // link slave specific setup to preop->safeop hook
                        ec_slave[slc].PO2SOconfig = &AEPsetup;
                    }
                }
            }


            ec_config_map(&IOmap);

            ec_configdc();

            printf("Slaves mapped, state to SAFE_OP.\n");
            /* wait for all slaves to reach SAFE_OP state */
            ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);

            oloop = ec_slave[0].Obytes;
            if ((oloop == 0) && (ec_slave[0].Obits > 0)) oloop = 1;
            if (oloop > 8) oloop = 8;
            iloop = ec_slave[0].Ibytes;
            if ((iloop == 0) && (ec_slave[0].Ibits > 0)) iloop = 1;
            if (iloop > 8) iloop = 8;

            printf("segments : %d : %d %d %d %d\n",ec_group[0].nsegments ,ec_group[0].IOsegment[0],ec_group[0].IOsegment[1],ec_group[0].IOsegment[2],ec_group[0].IOsegment[3]);

            printf("Request operational state for all slaves\n");
            expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
            printf("Calculated workcounter %d\n", expectedWKC);
            ec_slave[0].state = EC_STATE_OPERATIONAL;
            /* send one valid process data to make outputs in slaves happy*/
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);

            /* start RT thread as periodic MM timer */
            mmResult = timeSetEvent(1, 0, RTthread, 0, TIME_PERIODIC);

            /* request OP state for all slaves */
            ec_writestate(0);
            chk = 200;
            /* wait for all slaves to reach OP state */
            do
            {
                ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
            }
            while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
            if (ec_slave[0].state == EC_STATE_OPERATIONAL )
            {
                printf("Operational state reached for all slaves.\n");
                wkc_count = 0;
                inOP = TRUE;


                /* cyclic loop, reads data from RT thread */
                for(i = 1; i <= 500; i++)
                {
                    if(wkc >= expectedWKC)
                    {
                        printf("Processdata cycle %4d, WKC %d , O:", rtcnt, wkc);

                        for(j = 0 ; j < oloop; j++)
                        {
                            printf(" %2.2x", *(ec_slave[0].outputs + j));
                        }

                        printf(" I:");
                        for(j = 0 ; j < iloop; j++)
                        {
                            printf(" %2.2x", *(ec_slave[0].inputs + j));
                        }
                        printf(" T:%lld\r",ec_DCtime);
                        needlf = TRUE;
                    }
                    osal_usleep(50000);

                }
                inOP = FALSE;
            }
            else
            {
                printf("Not all slaves reached operational state.\n");
                ec_readstate();
                for(i = 1; i<=ec_slavecount ; i++)
                {
                    if(ec_slave[i].state != EC_STATE_OPERATIONAL)
                    {
                        printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                               i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
            }

            /* stop RT thread */
            timeKillEvent(mmResult);

            printf("\nRequest init state for all slaves\n");
            ec_slave[0].state = EC_STATE_INIT;
            /* request INIT state for all slaves */
            ec_writestate(0);
        }
        else
        {
            printf("No slaves found!\n");
        }
        printf("End simple test, close socket\n");
        /* stop SOEM, close socket */
        ec_close();
    }
    else
    {
        printf("No socket connection on %s\nExcecute as root\n",ifname);
    }
}

//DWORD WINAPI ecatcheck( LPVOID lpParam )
OSAL_THREAD_FUNC ecatcheck(void *lpParam)
{
    int slave;

    while(1)
    {
        if( inOP && ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate))
        {
            if (needlf)
            {
                needlf = FALSE;
                printf("\n");
            }
            /* one ore more slaves are not responding */
            ec_group[currentgroup].docheckstate = FALSE;
            ec_readstate();
            for (slave = 1; slave <= ec_slavecount; slave++)
            {
                if ((ec_slave[slave].group == currentgroup) && (ec_slave[slave].state != EC_STATE_OPERATIONAL))
                {
                    ec_group[currentgroup].docheckstate = TRUE;
                    if (ec_slave[slave].state == (EC_STATE_SAFE_OP + EC_STATE_ERROR))
                    {
                        printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slave);
                        ec_slave[slave].state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
                        ec_writestate(slave);
                    }
                    else if(ec_slave[slave].state == EC_STATE_SAFE_OP)
                    {
                        printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
                        ec_slave[slave].state = EC_STATE_OPERATIONAL;
                        ec_writestate(slave);
                    }
                    else if(ec_slave[slave].state > EC_STATE_NONE)
                    {
                        if (ec_reconfig_slave(slave, EC_TIMEOUTMON))
                        {
                            ec_slave[slave].islost = FALSE;
                            printf("MESSAGE : slave %d reconfigured\n",slave);
                        }
                    }
                    else if(!ec_slave[slave].islost)
                    {
                        /* re-check state */
                        ec_statecheck(slave, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
                        if (ec_slave[slave].state == EC_STATE_NONE)
                        {
                            ec_slave[slave].islost = TRUE;
                            printf("ERROR : slave %d lost\n",slave);
                        }
                    }
                }
                if (ec_slave[slave].islost)
                {
                    if(ec_slave[slave].state == EC_STATE_NONE)
                    {
                        if (ec_recover_slave(slave, EC_TIMEOUTMON))
                        {
                            ec_slave[slave].islost = FALSE;
                            printf("MESSAGE : slave %d recovered\n",slave);
                        }
                    }
                    else
                    {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d found\n",slave);
                    }
                }
            }
            if(!ec_group[currentgroup].docheckstate)
                printf("OK : all slaves resumed OPERATIONAL.\n");
        }
        osal_usleep(10000);
    }

    return 0;
}

char ifbuf[1024];

int main(int argc, char *argv[])
{
    ec_adaptert * adapter = NULL;
    printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");

    if (argc > 1)
    {
        /* create thread to handle slave error handling in OP */
        osal_thread_create(&thread1, 128000, &ecatcheck, (void*) &ctime);
        strcpy(ifbuf, argv[1]);
        /* start cyclic part */
        simpletest(ifbuf);
    }
    else
    {
        printf("Usage: simple_test ifname1\n");
        /* Print the list */
        printf ("Available adapters\n");
        adapter = ec_find_adapters ();
        while (adapter != NULL)
        {
            printf ("Description : %s, Device to use for wpcap: %s\n", adapter->desc,adapter->name);
            adapter = adapter->next;
        }
    }

    printf("End program\n");
    return (0);
}

