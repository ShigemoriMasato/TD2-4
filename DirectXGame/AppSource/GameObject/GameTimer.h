#pragma once

class GameTimer {
public:

	void Initialize();
	void Update(float deltaTime);
	bool IsEnd() const;
	float GetTimer() const { return timer_; }

private:

	float timer_ = 0.0f;

};
