#pragma once
#include <Core/DXDevice.h>
#include <Core/Command/CommandManager.h>
#include <Render/Screen/WindowsAPI.h>

namespace SHEngine {
	class ImGuiWrapper {
	public:

		void Initialize(DXDevice* device, Command::Manager* manager, Screen::WindowsAPI* window);

		void NewFrame();

		void Render();

		void Finalize();

	private:

		Logger logger_;

		SRVManager* srv_;
		std::vector<SRVHandle> srvHandles_;

		std::unique_ptr<Command::Object> cmdObject_;

		constexpr static int bufferNum_ = 3;
	};
}
