#pragma once
#include <vector>
#include <Sound/OpenALSource.h>
namespace g2 {
struct SourcePool {
	SourcePool() = default;
	SourcePool(int max_sources);

	~SourcePool();

	Source request_source(int priority);

private:
	std::vector<unsigned> free_sources;

	std::vector<unsigned> used_sources;
	std::vector<int>      priorities;
	//std::vector<int>  version;
};

} //namespace g2
