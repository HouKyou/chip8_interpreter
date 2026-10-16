#pragma once

class Chip8;
class Debugger {
public:
	void pause() { pause_ = true; }
	void resume() { pause_ = false; }
	void requestStep() { stepRequest_ = true; }
	bool takeStepRequest() {
		bool result = stepRequest_; 
		stepRequest_ = false;
		return result;
	}
	bool isPaused() const { return pause_; }
	
	void printState(const Chip8& chip8);
private:
	bool pause_{ false };
	bool stepRequest_{ false };
};
