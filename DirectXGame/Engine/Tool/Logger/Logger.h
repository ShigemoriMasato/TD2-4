#pragma once
#include <spdlog/spdlog.h>
#include <string>

using Logger = std::shared_ptr<spdlog::logger>;

	enum LoggerFlag : uint32_t {
		None = 0,
		MakeFile = 1 << 0,
		UseDebugString = 1 << 1,
		UseConsole = 1 << 2,
	};

Logger getLogger(const std::string& name, uint32_t flug = MakeFile | UseDebugString);

