#pragma once

#include <string>
#include <unordered_map>

// structures for keeping track of and storing level statistics

struct Stats {
	unsigned time = 0; // ticks
	unsigned jumps = 0;
	unsigned double_jumps = 0;
	int deaths = 0;
	int restarts = 0;

	constexpr unsigned total_jumps() const {
		return jumps + double_jumps;
	}
	constexpr unsigned total_respawns() const {
		return deaths + restarts;
	}

	struct Stats operator+(const Stats &other) const {
		return {
			time + other.time,
			jumps + other.jumps,
			double_jumps + other.double_jumps,
			deaths + other.deaths,
			restarts + other.restarts,
		};
	}
	struct Stats &operator+=(const Stats &other) {
		time += other.time;
		jumps += other.jumps;
		double_jumps += other.double_jumps;
		deaths += other.deaths;
		restarts += other.restarts;
		return *this;
	}

	bool better_than(const Stats &other) const {
		if (time < other.time) return true;
		else if (total_respawns() < other.total_respawns()) return true;
		else if (total_jumps() < other.total_jumps()) return true;
		return false;
	}
};

class PBFile {
	std::unordered_map<std::string, Stats> pbs{};

	static PBFile load(std::istream &inp);
	void save(std::ostream &out) const;
public:
	static PBFile load();
	void save() const;

	bool has_pb(std::string key) const;
	const Stats *get(std::string key) const;
	void set(std::string key, Stats val);
};
