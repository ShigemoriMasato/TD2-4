#pragma once
#include <Tool/Logger/Logger.h>

class ComputeObject {
public:

	ComputeObject(std::string debugName = "");
	~ComputeObject() = default;

	void Initialize();
	
private:

	Logger logger_ = getLogger("Compute");

};
