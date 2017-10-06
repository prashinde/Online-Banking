//
//  main.cpp
//  BankAccountManager
//
//  Created by tiantian xie on 2/24/17.
//  Copyright © 2017 tiantian xie. All rights reserved.
//

#include <iostream>


//#define SERVER

//#define TEST

#ifdef SERVER
#include "BankServer.hpp"
#else

#ifdef CLIENT
#include "Client.hpp"
#else
#include "TestRunner.hpp"
#endif
#endif




int main(int argc, const char * argv[]) {
    
    
#ifdef SERVER
    //./Server <port> <recordfilename>
    
    int port;
    std::string filename;
    if(argc<3){
        port=34567;
        filename="Records.txt";
    }else{
        port=std::atoi(argv[1]);
        filename=argv[2];
    }
    
    Server s;
    s.setPort(port);
    s.setListenNum(100);
    if(s.loadRecords(filename))
    {
        if(s.init()){
            s.mainLoop();
        }
    }
    return 0;
#endif
    
#ifdef CLIENT
    //./Client <clientid> <address> <port> <timestep> <transaction filename>
    
    int port,clientId;
    float timestep;
    std::string filename;
    std::string address;
    if(argc<6){
        clientId=rand()%1000;
        port=34567;
        timestep=0.001;//1ms
        address="127.0.0.1";
        filename="Transactions.txt";
    }else{
        clientId=std::atoi(argv[1]);
        address=argv[2];
        port=std::atoi(argv[3]);
        timestep=std::atof(argv[4]);
        filename=argv[5];
    }
    
    Agent a;
    
    a.setClientId(clientId);
    if(a.loadSimulationData(filename))
    {
        a.setTimeStep(timestep);
        a.run(address,port);
    }
    return 0;
#endif
#ifdef TEST
    Create_TestCase1();
    Create_TestCase2();
    Create_TestCase3();
    Create_TestCase4();
    
#endif
    
}
