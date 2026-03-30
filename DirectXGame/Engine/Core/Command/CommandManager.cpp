#include "CommandManager.h"

using namespace SHEngine::Command;

void Manager::Initialize(DXDevice* device) {
	device_ = device;
	logger_ = getLogger("Engine");

	//=========================================
	//コマンドキューの生成
	//=========================================
	auto& directQueue = queue_[Type::Direct].emplace_back();
	directQueue = std::make_unique<Queue>(device, Type::Direct);

	queue_[Type::Compute].reserve(6);
	for (int i = 0; i < 6; ++i) {
		auto& computeQueue = queue_[Type::Compute].emplace_back();
		computeQueue = std::make_unique<Queue>(device, Type::Compute);
	}

	//=========================================
	//コマンドオブジェクトの初期化
	//=========================================
	objects_[Type::Direct].resize(1);
	objects_[Type::Direct].resize(6);
}

std::unique_ptr<Object> SHEngine::Command::Manager::CreateCommandObject(Type type, int index, int listNum) {
	QueueChecker;

	auto queue = queue_[type][index].get();
	std::unique_ptr<Object> commandObject = std::make_unique<Object>(device_, type, queue, listNum);

	queue->RegisterObject(commandObject.get());

	return std::move(commandObject);
}

void SHEngine::Command::Manager::Execute(Type type, int index, std::vector<CmdObj*> cmdObj) {
	QueueChecker;

	auto& cmdQueue = queue_[type][index];
	cmdQueue->Execute(cmdObj);
}
