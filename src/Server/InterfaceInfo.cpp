//
// Created by csepsk on 2022/5/27.
//

#include "InterfaceInfo.h"

int getPrefixLength(in_addr mask){
    uint32_t n = ntohl(mask.s_addr);
    int i;
    for(i=0; n > 0; i++){
        n <<= 1;
    }
    return i;
}

bool InterfaceInfo::isIntranet(const in_addr subnet){
    const in_addr intranet2{.s_addr=htonl(0xAC100000)};
    const in_addr intranet2Mask{.s_addr=htonl(0xFFF00000)};
    const in_addr intranet3{.s_addr=htonl(0xC0A80000)};
    const in_addr intranet3Mask{.s_addr=htonl(0xFFFF0000)};
    if((subnet.s_addr & intranet2Mask.s_addr) == intranet2.s_addr)
        return true;
    if((subnet.s_addr & intranet3Mask.s_addr) == intranet3.s_addr)
        return true;
    return false;
}

void InterfaceInfo::Init(){
    struct ifaddrs *ifList;

    if(getifaddrs(&ifList) < 0){
        exit(-1);
    }

    for(auto ifa = ifList; ifa != nullptr; ifa = ifa->ifa_next){
        if(ifa->ifa_addr != nullptr && ifa->ifa_addr->sa_family == AF_INET){
            in_addr mask = ((struct sockaddr_in*)ifa->ifa_netmask)->sin_addr;
            in_addr ip = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            in_addr subnet = {.s_addr = mask.s_addr & ip.s_addr};
            // 自动代理172.16.0.0/12 192.168.0.0/16 网络内容
            if(isIntranet(subnet)){
                intranet[intranetCnt++]={.IP=subnet,.netmask=mask};
                fprintf(stdout,"已代理子网: %s/%d\n",inet_ntoa(subnet), getPrefixLength(mask));
            }else if(subnet.s_addr != 0){
                extraIP[extraIPCnt++]=ip;
            }
        }
    }
}

uint8_t InterfaceInfo::GetIntranet(Subnet *dest) {
    memcpy(dest,intranet,sizeof(Subnet)*intranetCnt);
    return intranetCnt;
}

void InterfaceInfo::PrintExtraIP(uint16_t portNum) {
    for(int i=0;i<extraIPCnt;i++){
        printf("%s:%d\n", inet_ntoa(extraIP[i]),portNum);
    }
}