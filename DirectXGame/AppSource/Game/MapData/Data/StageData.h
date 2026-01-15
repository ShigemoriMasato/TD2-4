#pragma once

enum class Direction {
	Front,
	Right,
	Back,
	Left,
};

struct StageData {
	int stageID = 0;
	int mapID = 0;
	Direction direction = Direction::Front;
	float goldFrequency = 1.0f;
};
