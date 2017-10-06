//
//  TestRunner.cpp
//  BankAccountManager
//
//  Created by tiantian xie on 2/24/17.
//  Copyright © 2017 tiantian xie. All rights reserved.
//

#include "TestRunner.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <arpa/inet.h>
#include <unistd.h> ////for read socketaddr_in
#include <netinet/in.h>

#include <fstream>

#ifdef S2M
	#define S2M	1000
#else 
	#define S2M	1 	//default using seconds, 
#endif

int PORT_NUM=14563;
//============
//Evaluation
//  CASE1: deposit successfully only when funds available.
//      funds only available to deposit twice.

void Create_TestCase1()
{
    system("mkdir CASE1");
    //test case to show if
    std::ofstream out("./CASE1/Records.txt");
    out<<"101 Peter 300";
    out.flush();
    out.close();
    
    std::ofstream out2("./CASE1/Transactions.txt");
    out2<<"10 101 w 150"<<std::endl;
    out2<<"11 101 w 150"<<std::endl;
    out2<<"12 101 w 120"<<std::endl;
    out2<<"13 101 d 3"<<std::endl;
    out2<<"14 101 w 4"<<std::endl;
    
    
    out2.flush();
    out2.close();
    
    //get the active address
    struct ifaddrs *addrs;
    getifaddrs(&addrs);
    struct ifaddrs* tmp;
    std::string address="";
    tmp=addrs;
    while(tmp){
        if(tmp->ifa_addr&&tmp->ifa_addr->sa_family==AF_INET
           &&(tmp->ifa_flags&IFF_RUNNING)&&(!(tmp->ifa_flags&IFF_LOOPBACK))){
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            address=inet_ntoa(pAddr->sin_addr);
            if(address=="127.0.0.1")
                continue;
            break;
        }
        tmp=tmp->ifa_next;
    }
    freeifaddrs(addrs);
    if(address==""){
        address="127.0.0.1";
    }
    srand(time(NULL));
    PORT_NUM=rand()%10000+1000;
    std::string script="#!/bin/bash\n\n";
    script+="./server ./CASE1/Records.txt "+address+" "+std::to_string(PORT_NUM)+" "+std::to_string(0)+" "+std::to_string(0)+" &";
    script+="(sleep 2; ./client ./CASE1/Transactions.txt 2000 "+address+" "+std::to_string(PORT_NUM)+" "+std::to_string(1)+" "+std::to_string(1000)+" "+std::to_string(1)+" &)";
 
    std::ofstream s_out("./case1.sh");
    s_out<<script<<std::endl;
    s_out.flush();
    s_out.close();
}

//  CASE2: Accepts requests concurrently.
//      one thread increase 10000 times, another thread decrease 10000 times, every 0.001second,
void Create_TestCase2()
{
    system("mkdir CASE2");
    
    std::ofstream out("./CASE2/Records.txt");
    out<<"101 Peter 10000\n";
    out<<"102 John 10000";
    
    out.flush();
    out.close();
    
    std::ofstream out2("./CASE2/Transactions.txt");
    for(int i=0;i<20000;++i){
        if(i%4==0)
            out2<<i<<" "<<"101"<<" w "<<1<<std::endl;
        else if(i%4==1)
            out2<<i<<" "<<"101"<<" d "<<1<<std::endl;
        else if(i%4==2)
            out2<<i<<" "<<"102"<<" d "<<1<<std::endl;
        else
            out2<<i<<" "<<"102"<<" w "<<1<<std::endl;
    }
    
    out2.flush();
    out2.close();
    
    //generate the test script
    //one server, two client, the result should be $10000 for both accounts.
    PORT_NUM=rand()%10000+2000;
    std::string script="#!/bin/bash\n\n";
    
    struct ifaddrs *addrs;
    getifaddrs(&addrs);
    struct ifaddrs* tmp;
    std::string address="";
    tmp=addrs;
    while(tmp){
        if(tmp->ifa_addr&&tmp->ifa_addr->sa_family==AF_INET
           &&(tmp->ifa_flags&IFF_RUNNING)&&(!(tmp->ifa_flags&IFF_LOOPBACK))){
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            address=inet_ntoa(pAddr->sin_addr);
            break;
        }
        tmp=tmp->ifa_next;
    }
    freeifaddrs(addrs);
    if(address==""){
        address="127.0.0.1";
    }
    
    script+="./server ./CASE2/Records.txt "+address+" "+std::to_string(PORT_NUM)+" "+std::to_string(0)+" "+std::to_string(0)+" &";
    script+="(sleep 2; ./client ./CASE2/Transactions.txt 2000 "+address+" "+std::to_string(PORT_NUM)+" "+std::to_string(1)+" "+std::to_string(1000)+" "+std::to_string(1)+" )&";
    script+="(sleep 2; ./client ./CASE2/Transactions.txt 2000 "+address+" "+std::to_string(PORT_NUM)+" "+std::to_string(1)+" "+std::to_string(1000)+" "+std::to_string(2)+" &)";
    
    std::ofstream s_out("./case2.sh");
    s_out<<script<<std::endl;
    s_out.flush();
    s_out.close();
}


std::string create_running_for(int clientId,int process_num,double timestep,std::string address,std::string dir);
//  GLOBAL:
//  CASE3: from 1 to 100 number of clients sending in 2 seconds
void Create_TestCase3()
{
    system("mkdir CASE3");
    
    std::ofstream out("./CASE3/Records.txt");
    out<<"101 Peter 16000\n102 John 1200\n103 Gambo 11000\n105 Jacob 19890\n";

    
    out.flush();
    out.close();
    
    std::ofstream out2("./CASE3/Transactions.txt");
    out2<<"10 101 w 200\n25 103 d 300\n26 102 d 150\n27 103 d 300\n29 101 d 200\n31 102 w 149\n32 102 d 12";
    
    
    out2.flush();
    out2.close();
    
    //generate the test script
    //one server, two client, the result should be $10000 for both accounts.
    
    struct ifaddrs *addrs;
    getifaddrs(&addrs);
    struct ifaddrs* tmp;
    std::string address="";
    tmp=addrs;
    while(tmp){
        if(tmp->ifa_addr&&tmp->ifa_addr->sa_family==AF_INET
           &&(tmp->ifa_flags&IFF_RUNNING)&&(!(tmp->ifa_flags&IFF_LOOPBACK))){
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            address=inet_ntoa(pAddr->sin_addr);
            break;
        }
        tmp=tmp->ifa_next;
    }
    freeifaddrs(addrs);
    if(address==""){
        address="127.0.0.1";
    }
    
    //create all test cases varying number of process
    PORT_NUM=rand()%10000+3000;
    std::string filelist;
    std::string runningcmds="#!/bin/bash\n\n";
    runningcmds+="mkdir case3_result\n";
    for(int i=0;i<100;i+=5)
    {
        std::string script=create_running_for(i,5+i, 1*S2M, address,"CASE3");
        std::string title="case3_"+std::to_string(5+i)+".sh";
        filelist+=title+" ";
        std::ofstream out_sh(title);
        out_sh<<script<<std::endl;
        out_sh.flush();
        out_sh.close();
        
        runningcmds+="./"+title+" > ./case3_result/"+std::to_string(5+i)+".txt\n";
        runningcmds+="sudo pkill server ; echo ...\n";

    }
    
    //save the file list
    std::ofstream flistfile("case3_filelist.txt");
    flistfile<<filelist<<" case3.sh";
    flistfile.flush();
    flistfile.close();
    
    //create a running file for all sh
    std::ofstream case3_out("case3.sh");
    case3_out<<runningcmds;
    case3_out.flush();
    case3_out.close();
    
    //how to execute
    //step 1: chmod +x `cat case3_filelist.txt`
    //step 2: ./case3.sh
    //the result is inside folder case3_result. needs to implement another program to plot the result
    //track line started with `avg`
    //e.g., avg 1 1  means   the average running time for a transaction on client 1 is 1s.
}

void Create_TestCase4()
{
    system("mkdir CASE4");
    
    std::ofstream out("./CASE4/Records.txt");
    out<<"101 Peter 16000\n102 John 1200\n103 Gambo 11000\n105 Jacob 19890\n";
    
    
    out.flush();
    out.close();
    
    std::ofstream out2("./CASE4/Transactions.txt");
    out2<<"10 101 w 200\n25 103 d 300\n26 102 d 150\n27 103 d 300\n29 101 d 200\n31 102 w 149\n32 102 d 12";
    
    out2.flush();
    out2.close();
    
    //generate the test script
    //one server, two client, the result should be $10000 for both accounts.
    
    struct ifaddrs *addrs;
    getifaddrs(&addrs);
    struct ifaddrs* tmp;
    std::string address="";
    tmp=addrs;
    while(tmp){
        if(tmp->ifa_addr&&tmp->ifa_addr->sa_family==AF_INET
           &&(tmp->ifa_flags&IFF_RUNNING)&&(!(tmp->ifa_flags&IFF_LOOPBACK))){
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            address=inet_ntoa(pAddr->sin_addr);
            break;
        }
        tmp=tmp->ifa_next;
    }
    freeifaddrs(addrs);
    if(address==""){
        address="127.0.0.1";
    }
    
    //create all test cases varying number of process
    PORT_NUM=rand()%10000+4000;
    std::string filelist;
    std::string runningcmds="#!/bin/bash\n\n";
    runningcmds+="mkdir case4_result\n";
    for(int i=0;i<100;i+=10)
    {
        std::string script=create_running_for(i,25, double(10+i)/100*S2M,address,"CASE4");
        std::string title="case4_"+std::to_string(10+i)+".sh";
        filelist+=title+" ";
        std::ofstream out_sh(title);
        out_sh<<script<<std::endl;
        out_sh.flush();
        out_sh.close();
        
        runningcmds+="./"+title+" > ./case4_result/"+std::to_string(10+i)+".txt\n";
        runningcmds+="sudo pkill server ; echo ...\n";
    }
    
    //save the file list
    std::ofstream flistfile("case4_filelist.txt");
    flistfile<<filelist<<" case4.sh";
    flistfile.flush();
    flistfile.close();
    
    //create a running file for all sh
    std::ofstream case3_out("case4.sh");
    case3_out<<runningcmds;
    case3_out.flush();
    case3_out.close();
    
    //how to execute
    //step 1: chmod +x `cat case4_filelist.txt`
    //step 2: ./case4.sh
    //the result is inside folder case4_result. needs to implement another program to plot the result
    //track line started with `avg`
    //e.g., avg 1 1  means   the average running time for a transaction on client 1 is 1s.
}



std::string create_running_for(int clientId,int process_num,double timestep,std::string address,std::string dir)
{
    std::string script="#!/bin/bash\n\n";
    script+="./server "+dir+"/Records.txt "+address+" "+std::to_string(PORT_NUM)+" "+std::to_string(0)+" "+std::to_string(0)+"&";
    int id=0;
    for(int i=0;i<process_num-1;++i){
        script+="(sleep 2; ./client "+dir+"/Transactions.txt 2000 "+address+" "+std::to_string(PORT_NUM)+" "+std::to_string(1)+" "+std::to_string(1000)+" "+std::to_string(id)+" )&";
        id++;
    }
         script+="(sleep 2; ./client "+dir+"/Transactions.txt 2000 "+address+" "+std::to_string(PORT_NUM)+" "+std::to_string(1)+" "+std::to_string(1000)+" "+std::to_string(id) +" &)";
 
    return script;
}


//  CASE4: from 0.1, 0.2, ..1 seconds 25 clients
