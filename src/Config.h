#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <random>

enum ConfigElement {
		SEED,
		INIT_TIME,
		FIN_TIME,
		ARRIVE_MIN,
		ARRIVE_MAX,
		QUIT_PROB,
		CPU_MIN,
		CPU_MAX,
		DISK_MIN,
		DISK_MAX,
		CORES,
		DISKS,
		REALTIME,
		TICKRATE,
};

int find(const std::string&, char);
	
struct Config {

	std::vector<int> configValues;

	int get_config_value(ConfigElement e) {
		return this->configValues.at((int)e);
	}

	int get_range(ConfigElement min, ConfigElement max) {
		int time_min = this->get_config_value(min);
    	int time_max = this->get_config_value(max);
		return time_min+rand()%(time_max-time_min+1);
	}

	bool import_config_from_file(const std::string& filename) {
		std::string line;
		std::ifstream f(filename);
		//Order in config matters!!! Must match order in enum
		if(f.is_open()) {
			while(getline(f,line)) {
				if(find(line,'#') > 0) {
					continue;
				}
				this->configValues.push_back(std::stoi(line.substr(find(line,' '),line.size())));
			}
			f.close();
			return true;
		} else {
			return false;
		}
	}

	Config() {
	}

};
