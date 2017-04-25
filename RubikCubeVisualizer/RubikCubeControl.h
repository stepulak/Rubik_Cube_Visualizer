#ifndef RUBIK_CUBE_CONTROL_H
#define RUBIK_CUBE_CONTROL_H

#include "RubikCube.h"
#include <memory>
#include <thread>
#include <list>

// Rubik's cube control center
// It runs in separate thread
class RubikCubeControl final {
private:

	std::shared_ptr<RubikCube> m_rubikCube;
	std::thread m_thread;
	bool m_running;
	std::list<std::string> m_rotationHistory;

	void HandleCommand();
	void PrintHelp() const;
	void Rotate(const std::string& command, bool saveCommandToHistory = true);
	void UndoRotation();
	void SaveRotations(const std::string& filename) const;
	void LoadAndPerformRotations(const std::string& filename);

public:

	RubikCubeControl(std::shared_ptr<RubikCube> rubikCube);
	~RubikCubeControl();

	inline bool IsRunning() const { return m_running; }
};

#endif