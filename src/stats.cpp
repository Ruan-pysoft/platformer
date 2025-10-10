#include "stats.hpp"
#include "globals.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

static void trim(std::string &str) {
	auto frst = str.find_first_not_of(" \r\t");
	auto last = str.find_last_not_of(" \r\t");

	if (frst == std::string::npos) str = "";
	else str = str.substr(frst, last - frst + 1);
}

PBFile PBFile::load(std::istream &inp) {
	PBFile res{};

	std::string line;
	Stats *curr = nullptr;

	while (std::getline(inp, line)) {
		trim(line);
		if (line.empty() || line[0] == '#') continue;
		auto pos = line.find(' ');

		if (pos == std::string::npos && line == "END") {
			curr = nullptr;
			continue;
		} else if (pos == std::string::npos) {
			std::cerr << "Warning: malformed line in personal bests file, skipping!" << std::endl;
			continue;
		}
		std::string key = line.substr(0, pos);
		std::string val = line.substr(pos + 1);
		trim(key);
		trim(val);

		if (key == "BEGIN") {
			if (curr != nullptr) {
				std::cerr << "Warning: started new PB list before ending the last one!" << std::endl;
			}
			if (res.pbs.find(val) == res.pbs.end()) {
				res.pbs[val] = {};
			}
			curr = &res.pbs[val];
		} else if (key == "time") {
			if (curr == nullptr) {
				std::cerr << "Warning: time specified outside of PB list!" << std::endl;
				continue;
			}
			curr->time = atoi(val.c_str());
		} else if (key == "jumps") {
			if (curr == nullptr) {
				std::cerr << "Warning: jumps specified outside of PB list!" << std::endl;
				continue;
			}
			curr->jumps = atoi(val.c_str());
		} else if (key == "double_jumps") {
			if (curr == nullptr) {
				std::cerr << "Warning: double jumps specified outside of PB list!" << std::endl;
				continue;
			}
			curr->double_jumps = atoi(val.c_str());
		} else if (key == "deaths") {
			if (curr == nullptr) {
				std::cerr << "Warning: deaths specified outside of PB list!" << std::endl;
				continue;
			}
			curr->deaths = atoi(val.c_str());
		} else if (key == "restarts") {
			if (curr == nullptr) {
				std::cerr << "Warning: restarts specified outside of PB list!" << std::endl;
				continue;
			}
			curr->restarts = atoi(val.c_str());
		} else {
			std::cerr << "Warning: unrecognised key " << key << " in PB file!" << std::endl;
			continue;
		}
	};

	return res;
}
void PBFile::save(std::ostream &out) const {
	for (const auto &item : pbs) {
		out << "BEGIN " << item.first << std::endl;
		out << "time " << item.second.time << std::endl;
		out << "jumps " << item.second.jumps << std::endl;
		out << "double_jumps " << item.second.double_jumps << std::endl;
		out << "deaths " << item.second.deaths << std::endl;
		out << "restarts " << item.second.restarts << std::endl;
		out << "END" << std::endl;
	}
}
PBFile PBFile::load() {
	if (!std::filesystem::exists(global::DATA_DIR)) {
		if (!std::filesystem::create_directory(global::DATA_DIR)) {
			std::cerr << "Failed creating game data folder!" << std::endl;
			return {};
		}
	}
	std::string pbs_file_name;
	pbs_file_name += global::DATA_DIR;
	pbs_file_name += global::PERSONAL_BESTS_FILE;
	if (std::filesystem::exists(pbs_file_name)) {
		std::ifstream pbs_file(pbs_file_name);
		PBFile res = load(pbs_file);
		pbs_file.close();
		return res;
	}
	return {};
}
void PBFile::save() const {
	if (!std::filesystem::exists(global::DATA_DIR)) {
		if (!std::filesystem::create_directory(global::DATA_DIR)) {
			std::cerr << "Failed creating game data folder!" << std::endl;
			return;
		}
	}

	std::string pbs_file_name;
	pbs_file_name += global::DATA_DIR;
	pbs_file_name += global::PERSONAL_BESTS_FILE;

	std::ofstream pbs_file(pbs_file_name);
	save(pbs_file);
	pbs_file.close();
}

bool PBFile::has_pb(std::string key) const {
	return pbs.find(key) != pbs.end();
}
const Stats *PBFile::get(std::string key) const {
	const auto found = pbs.find(key);
	if (found == pbs.end()) {
		return nullptr;
	} else {
		return &found->second;
	}
}
void PBFile::set(std::string key, Stats val) {
	pbs[key] = val;
}
