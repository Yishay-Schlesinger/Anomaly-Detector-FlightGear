//208438119
#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <fstream>
#include <iostream>
#include <string.h>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <stdexcept>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "HybridAnomalyDetector.h"

using namespace std;

class SharedContent
{
public:
	HybridAnomalyDetector hybrid_ad;
	vector<AnomalyReport> anomaly_report;
	int last_detect_ts_size; //size of the detect ts, that the anomaly_report made from.
	string anomaly_train_filename = "anomalyTrain.csv";
	string anomaly_test_filename = "anomalyTest.csv";
	bool to_exit = false;
	string threePointAfterDot(float x)
	{
		x = x * 1000;
		x = floor(x);
		x = x / 1000;
		stringstream sstream;
		sstream << x;
		string x_str = sstream.str();
		return x_str;
	}
};

class DefaultIO
{
public:
	virtual string read() = 0;
	virtual void write(string text) = 0;
	virtual void write(float f) = 0;
	virtual void read(float *f) = 0;
	virtual ~DefaultIO() {}
	// you may add additional methods here
};

class SocketIO : public DefaultIO
{
	int socketID;
public:
	SocketIO(int clientID)
	{
		this->socketID = clientID;
	}
	virtual string read() { 
		string serverInput="";
		char c=0;
		::read(socketID,&c,sizeof(char));
		while(c!='\n'){				
			serverInput+=c;
			::read(socketID,&c,sizeof(char));
		}
		return serverInput;
	}
	virtual void write(string text)
	{
		::write(socketID,text.c_str(),text.length());
	}
	virtual void write(float f) {}
	virtual void read(float *f) {}
	void close()
	{}
	~SocketIO()
	{
		close();
	}
};

// you may add here helper classes

// you may edit this class
class Command
{
public:
	DefaultIO *dio;
	SharedContent *sc;
	string description;
	Command(DefaultIO *dio, string descript, SharedContent *sc) : dio(dio), description(descript)
	{
		this->sc = sc;
	}
	virtual void execute() = 0;
	virtual ~Command() {}
};

// you may add here helper classes

// you may edit this class

class UploadTsCommand : public Command
{
public:
	UploadTsCommand(DefaultIO *dio, SharedContent *sc) : Command(dio, "upload a time series csv file", sc) {}
	void execute()
	{
		string line = "";
		string state1 = "train", state2 = "test";
		string cmd_to_write1 = "Please upload your local ", cmd_to_write2 = " CSV file.\n";
		string cmd_up_complete = "Upload complete.\n";
		dio->write(cmd_to_write1 + state1 + cmd_to_write2);
		writeTo(sc->anomaly_train_filename);
		dio->write(cmd_up_complete);
		dio->write(cmd_to_write1 + state2 + cmd_to_write2);
		writeTo(sc->anomaly_test_filename);
		dio->write(cmd_up_complete);
	}

	void writeTo(string file_path)
	{
		ofstream out;
		out.open(file_path);
		string line = dio->read();
		while (line != "done")
		{
			out << line + "\n";
			line = dio->read();
		}
		if (out.is_open())
			out.close();
	}
};

class ThresholdStateCommand : public Command
{
public:
	ThresholdStateCommand(DefaultIO *dio, SharedContent *sc) : Command(dio, "algorithm settings", sc) {}
	void execute()
	{
		string current_threshold = sc->threePointAfterDot(sc->hybrid_ad.min_correlation);
		string cmd1 = "The current correlation threshold is " + current_threshold + "\n";
		string cmd2 = "Type a new threshold\n";
		string cmd3 = "please choose a value between 0 and 1.\n";
		dio->write(cmd1 + cmd2);
		float new_threshold;
		string u_c = dio->read();
		stringstream ss(u_c);
		ss >> new_threshold;
		while (!(0 < new_threshold && new_threshold < 1))
		{
			dio->write(cmd3);
			string u_c = dio->read();
			stringstream ss(u_c);
			ss >> new_threshold;
		}
		sc->hybrid_ad.min_correlation = new_threshold;
	}
};

class DetectAnomalyCommand : public Command
{
public:
	DetectAnomalyCommand(DefaultIO *dio, SharedContent *sc) : Command(dio, "detect anomalies", sc) {}
	void execute()
	{
		string cmd = "anomaly detection complete.\n";
		string learn_file = sc->anomaly_train_filename;
		string detect_file = sc->anomaly_test_filename;
		TimeSeries ts_learn(learn_file);
		TimeSeries ts_detect(detect_file);
		sc->last_detect_ts_size = ts_detect.get_colunms().begin()->second.size();
		sc->hybrid_ad.learnNormal(ts_learn);
		sc->anomaly_report = sc->hybrid_ad.detect(ts_detect);
		dio->write(cmd);
	}
};

class DisplayResultCommand : public Command
{
public:
	DisplayResultCommand(DefaultIO *dio, SharedContent *sc) : Command(dio, "display results", sc) {}
	void execute()
	{
		string cmd = "Done.\n";
		string learn_file = sc->anomaly_train_filename;
		string detect_file = sc->anomaly_test_filename;
		int my_ar_size = sc->anomaly_report.size();
		for (int i=0;i<my_ar_size;i++) 
		{
			dio->write(to_string(sc->anomaly_report[i].timeStep) + " \t" + sc->anomaly_report[i].description + "\n");
		}
		dio->write(cmd);
	}
};

class AnalyzeResultsCommand : public Command
{
public:
	AnalyzeResultsCommand(DefaultIO *dio, SharedContent *sc) : Command(dio, "upload anomalies and analyze results", sc) {}
	void execute()
	{
		string cmd1 = "Please upload your local anomalies file.\n";
		string cmd2 = "Upload complete.\n";
		string cmd3 = "True Positive Rate: ";
		string cmd4 = "False Positive Rate: ";
		string path = "analyzeAnomalyFile.csv";
		dio->write(cmd1);
		writeTo(path);
		dio->write(cmd2);
		TimeSeries ts(path);
		map<string, vector<float>> user_analyze_seq = ts.get_colunms();

		map<string, vector<float>> my_analyze_seqs;
		my_analyze_seqs["A"] = {};
		my_analyze_seqs["B"] = {};
		int my_ar_size = sc->anomaly_report.size();
		if (my_ar_size > 0)
		{
			string current_description = sc->anomaly_report[0].description;
			my_analyze_seqs["A"].push_back(sc->anomaly_report[0].timeStep);
			my_analyze_seqs["B"].push_back(sc->anomaly_report[0].timeStep);
			for (int i=1; i<my_ar_size; i++) 
			{
				if (sc->anomaly_report[i].description== current_description)
				{
					my_analyze_seqs["B"].pop_back();
					my_analyze_seqs["B"].push_back(sc->anomaly_report[i].timeStep);
				}
				else
				{
					my_analyze_seqs["A"].push_back(sc->anomaly_report[i].timeStep);
					my_analyze_seqs["B"].push_back(sc->anomaly_report[i].timeStep);
					current_description = sc->anomaly_report[i].description;
				}
			}
		}
		string A = "A", B = "B";
		int sum_user_alarams = 0;
		int positive = user_analyze_seq[A].size();
		int TruePositive = 0;
		int FalsePositive = 0;

		for (int i = 0; i < my_analyze_seqs[A].size(); i++)
		{
			for (int j = 0; j < positive; j++)
			{
				if (my_analyze_seqs[A][i] <= user_analyze_seq[A][j] && my_analyze_seqs[B][i] >= user_analyze_seq[A][j])
				{
					break;
				}
				else if (my_analyze_seqs[A][i] <= user_analyze_seq[B][j] && my_analyze_seqs[B][i] >= user_analyze_seq[B][j])
				{
					break;
				}
				else if (my_analyze_seqs[A][i] >= user_analyze_seq[A][j] && my_analyze_seqs[B][i] <= user_analyze_seq[B][j])
				{
					break;
				}
				else if (j == positive-1)
				{
					FalsePositive++;
				}
			}
		}

		for (int j = 0; j < positive; j++)
		{
			for (int i = 0; i < my_analyze_seqs[A].size(); i++)
			{
				if (my_analyze_seqs[A][i] <= user_analyze_seq[A][j] && my_analyze_seqs[B][i] >= user_analyze_seq[A][j])
				{
					TruePositive++;
					break;
				}
				else if (my_analyze_seqs[A][i] <= user_analyze_seq[B][j] && my_analyze_seqs[B][i] >= user_analyze_seq[B][j])
				{
					TruePositive++;
					break;
				}
				else if (my_analyze_seqs[A][i] >= user_analyze_seq[A][j] && my_analyze_seqs[B][i] <= user_analyze_seq[B][j])
				{
					TruePositive++;
					break;
				}
			}
		}


		for (int j = 0; j < positive; j++)
		{
			sum_user_alarams += user_analyze_seq[B][j] - user_analyze_seq[A][j] + 1;
		}

		float negetive = sc->last_detect_ts_size - sum_user_alarams;
		string true_positive_rate = sc->threePointAfterDot(float(TruePositive) / float(positive));
		string false_positive_rate = sc->threePointAfterDot(float(FalsePositive) / float(negetive));

		dio->write(cmd3 + true_positive_rate+ "\n");
		dio->write(cmd4 + false_positive_rate + "\n");
	}

	void writeTo(string file_path)
	{
		ofstream out;
		out.open(file_path);
		string line = "A,B";
		while (line != "done")
		{
			out << line + "\n";
			line = dio->read();
		}
		if (out.is_open())
			out.close();
	}
};

class ExitCommand : public Command
{
public:
	ExitCommand(DefaultIO *dio, SharedContent *sc) : Command(dio, "exit", sc) {}
	void execute()
	{
		sc->to_exit = true;
	}
};

#endif /* COMMANDS_H_ */