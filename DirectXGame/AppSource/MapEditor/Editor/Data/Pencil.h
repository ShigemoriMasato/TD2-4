#pragma once

namespace MapEditor {

	class Pencil {
	public:

		void Initialize();
		int& GetSelectedID();

	private:

		int selectedID_ = -1;

	};

}
