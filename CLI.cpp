//208438119

#include "CLI.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <map>
#include <stdexcept>

using namespace std;

CLI::CLI(DefaultIO* dio) {
    this->dio = dio;
    SharedContent* sc = new SharedContent();
    *sc = SharedContent();
    commands.push_back(new UploadTsCommand(dio,sc));
    commands.push_back(new ThresholdStateCommand(dio,sc));
    commands.push_back(new DetectAnomalyCommand(dio,sc));
    commands.push_back(new DisplayResultCommand(dio,sc));
    commands.push_back(new AnalyzeResultsCommand(dio,sc));
    commands.push_back(new ExitCommand(dio,sc));
}

void CLI::start(){
    string msg = "Welcome to the Anomaly Detection Server.\nPlease choose an option:\n";
    
    while (!commands[0]->sc->to_exit) 
    { 
        dio->write(msg);
        for(int i=0;i<commands.size();i++){
            dio->write(to_string(i+1) + "." +  commands[i]->description + "\n");
        }
        int user_choise = 0;
        string u_c = dio->read();
        stringstream ss(u_c);
        ss>>user_choise;
        commands[user_choise-1]->execute();
    }
}

CLI::~CLI()
{
    delete commands[0]->sc;
    for(int i=0;i<commands.size();i++){
            delete commands[i];
        }
}