#include <QCoreApplication>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <QDebug>
#include <unistd.h>
#include <string>



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << argv[1][0];
    int s;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Socket");
        return 1;
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0" );
    ioctl(s, SIOCGIFINDEX, &ifr);


    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
       perror("Bind");
       return 1;
    }

    // Sending a frame
    if (*argv[1] == '1') {
        int nbytes;
        qDebug() << "Sending a frame";
        struct can_frame frame;
        frame.can_id = 0x555;
        frame.can_dlc = 5;
        const char* str = "Hello";

        nbytes = sendto(s, &frame, sizeof(struct can_frame),
                        0, (struct sockaddr*)&addr, sizeof(addr));
//        sprintf((char*)frame.data, str);
//        if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
//           perror("Write");
//           return 1;
//        }
    }

    // Reading a frame
    if (*argv[1] == '2') {
        qDebug() << "Receiving a frame";
        int nbytes;
        struct can_frame frame;
        frame.can_id = 0x555;
        frame.can_dlc = 5;
        socklen_t len = sizeof(addr);
        qDebug() << "Receiving a frame 1";
//        nbytes = read(s, &frame, sizeof(struct can_frame));
        nbytes = recvfrom(s, &frame, sizeof(struct can_frame),
                          0, (struct sockaddr*)&addr, &len);
        qDebug() << "Receiving a frame 2";
        if (nbytes < 0) {
            qDebug() << "nbytes < 0";
            perror("Read");
            return 1;
        }
        printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);
        for (int i = 0; i < frame.can_dlc; i++)
            printf("%02X ",frame.data[i]);
        printf("\r\n");
    }
    if (close(s) < 0) {
       perror("Close");
       return 1;
    }
    return 1;
}
