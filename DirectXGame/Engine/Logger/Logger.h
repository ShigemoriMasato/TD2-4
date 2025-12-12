#pragma once
#include <spdlog/spdlog.h>
#include <string>

using Logger = std::shared_ptr<spdlog::logger>;

namespace LogSystem {

	enum LoggerFlag : uint32_t {
		None = 0,
		MakeFile = 1 << 0,
		UseConsole = 1 << 1,
	};

	void Initialize();

	Logger getLogger(const std::string& name, uint32_t flug = MakeFile | UseConsole);

}
