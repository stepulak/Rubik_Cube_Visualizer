#include "RubikCubeControl.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

RubikCubeControl::RubikCubeControl(const std::shared_ptr<RubikCube>& rubikCube)
	: m_rubikCube(rubikCube),
	m_running(true)
{
	m_thread = std::thread([this]() {
		std::cout << "Welcome to the Rubik's Cube Control Center!\n\n";
		std::cout << "Write help to show available commands.\n\n";
		
		while (m_running) {
			std::cout << "> ";
			HandleCommand();
		}
	});
}

RubikCubeControl::~RubikCubeControl()
{
	m_thread.detach();
}

void RubikCubeControl::HandleCommand()
{
	std::string command;
	std::cin >> command;

	try {
		if (command == "quit") {
			m_running = false;
		}
		else if (command == "help") {
			PrintHelp();
		}
		else if (command == "rotate") {
			if (m_rubikCube->IsRotating()) {
				std::cout << "Error: Rubik cube is rotating\n";
			}
			else {
				std::getline(std::cin, command);
				Rotate(command);
				std::cout << "Rotation performed\n";
			}
		}
		else if (command == "undo") {
			UndoRotation();
			std::cout << "Undo performed\n";
		}
		else if (command == "reset") {
			m_rubikCube->NewCube(m_rubikCube->GetNumStickersPerEdge());
			m_rotationHistory.clear();
			std::cout << "Cube was reset\n";
		}
		else if (command == "new_cube") {
			unsigned int numStickers;
			std::cin >> numStickers;
			m_rubikCube->NewCube(numStickers);
			m_rotationHistory.clear();
			std::cout << "Cube created\n";
		}
		else if (command == "save") {
			std::cin >> command; // get filename
			m_rubikCube->SaveIntoFile(command);
			std::cout << "Cube saved\n";
		}
		else if (command == "load") {
			std::cin >> command; // get filename
			m_rubikCube->LoadFromFile(command);
			m_rotationHistory.clear();
			std::cout << "Cube loaded\n";
		}
		else if (command == "save_rotations") {
			std::cin >> command; // get filename
			SaveRotations(command);
			std::cout << "Rotations saved\n";
		}
		else if (command == "load_rotations") {
			std::cin >> command; // get filename
			LoadAndPerformRotations(command);
			m_rotationHistory.clear();
			std::cout << "Rotations loaded and performed\n";
		}
		else {
			std::cout << "Unknown command. Type help for available commands.\n";
		}
	}
	catch (const std::exception& ex) {
		std::cout << "Error: " << ex.what() << std::endl;
	}
}

void RubikCubeControl::PrintHelp() const
{
	std::cout << "Available commands: \n\n";
	std::cout << "quit - quit this program\n\n";
	std::cout << "help - show this help\n\n";

	std::cout << "rotate [face] [. (optional)] [level (optinal)] - rotate cube\n";
	std::cout << "\t[face] F (Front), B (Back), U (Up), D (Down), L (Left), R (Right)\n";
	std::cout << "\t3x3x3 Cube: \n";
	std::cout << "\t\t[face] M (Middle layer, between Left and Right)\n";
	std::cout << "\t\t[face] E (Equator layer, between Up and Down)\n";
	std::cout << "\t\t[face] E (Standing layer, between Up and Down)\n";
	std::cout << "\tGeneric Cube: \n";
	std::cout << "\t\t[face] X [level] X axis rotation on specific level (0-N)\n";
	std::cout << "\t\t[face] Y [level] Y axis rotation on specific level (0-N)\n";
	std::cout << "\t\t[face] Z [level] Z axis rotation on specific level (0-N)\n";
	std::cout << "\tYou can write . symbol after each face to rotate counter-clockwise.\n";
	std::cout << "\tThe default rotation direction is clockwise.\n";
	std::cout << "\tExamples:\n";
	std::cout << "\t\trotate F\n\t\trotate M .\n\t\trotate X . 1\n\n";

	std::cout << "undo - undo previous rotation\n\n";
	std::cout << "reset - reset current Rubik's Cube configuration\n\n";
	std::cout << "new_cube [num_stickers] - create new Cube with specific number of stickers per edge\n\n";
	std::cout << "save [filename] - save current Rubik's Cube configuration into file\n\n";
	std::cout << "load [filename] - load Rubik's Cube configuration from file\n\n";
	std::cout << "save_rotations [filename] - save rotations history into file\n\n";
	std::cout << "load_rotations [filename] - load rotations from file and perform them\n\n";
}

void RubikCubeControl::Rotate(const std::string& command, bool saveCommandToHistory)
{
	static auto check333CubeOrThrow = [this]() { 
		if (m_rubikCube->GetNumStickersPerEdge() != 3) {
			throw std::runtime_error("This operator is for 3x3x3 Rubik's Cube only");
		}
	};

	auto commandWithoutSpaces = command;

	// remove spaces and shrink the string
	commandWithoutSpaces.erase(
		std::remove_if(commandWithoutSpaces.begin(), commandWithoutSpaces.end(), ::isspace),
		commandWithoutSpaces.end());
	
	char face = commandWithoutSpaces[0];
	bool clockwise = commandWithoutSpaces[1] != '.';
	bool needIndex = false;
	unsigned int rotationIndex = 0;

	if (face == 'X' || face == 'Y' || face == 'Z') {
		needIndex = true;
		auto pos = commandWithoutSpaces.find_first_of("0123456789");

		if (pos != std::string::npos) {
			rotationIndex = static_cast<unsigned int>(std::stol(commandWithoutSpaces.substr(pos)));
		}
		else {
			throw std::runtime_error("Missing rotation level");
		}
	}

	auto numStickers = m_rubikCube->GetNumStickersPerEdge();

	switch (face) {
	case 'F': // front
		m_rubikCube->Rotate(RubikCube::Z_AXIS, numStickers - 1, clockwise);
		break;
	case 'B': // back
		m_rubikCube->Rotate(RubikCube::Z_AXIS, 0, clockwise);
		break;
	case 'U': // up
		m_rubikCube->Rotate(RubikCube::Y_AXIS, numStickers - 1, clockwise);
		break;
	case 'D': // down
		m_rubikCube->Rotate(RubikCube::Y_AXIS, 0, clockwise);
		break;
	case 'L': // left
		m_rubikCube->Rotate(RubikCube::X_AXIS, 0, clockwise);
		break;
	case 'R': // right
		m_rubikCube->Rotate(RubikCube::X_AXIS, numStickers - 1, clockwise);
		break;
	case 'M': // x axis middle
		check333CubeOrThrow();
		m_rubikCube->Rotate(RubikCube::X_AXIS, 1, clockwise);
		break;
	case 'E': // y axis middle
		check333CubeOrThrow();
		m_rubikCube->Rotate(RubikCube::Y_AXIS, 1, clockwise);
		break;
	case 'S': // z axis middle
		check333CubeOrThrow();
		m_rubikCube->Rotate(RubikCube::Z_AXIS, 1, clockwise);
		break;
	case 'X':
		m_rubikCube->Rotate(RubikCube::X_AXIS, rotationIndex, clockwise);
		break;
	case 'Y':
		m_rubikCube->Rotate(RubikCube::Y_AXIS, rotationIndex, clockwise);
		break;
	case 'Z':
		m_rubikCube->Rotate(RubikCube::Z_AXIS, rotationIndex, clockwise);
		break;
	default:
		throw std::runtime_error("Unknown face");
		break;
	}

	if (saveCommandToHistory) {
		std::stringstream ss;
		ss << face;

		if (!clockwise) {
			ss << '.';
		}
		if (needIndex) {
			ss << rotationIndex;
		}

		m_rotationHistory.push_back(ss.str());
	}
}

void RubikCubeControl::UndoRotation()
{
	if (m_rotationHistory.empty()) {
		throw std::runtime_error("Rotation history is empty");
	}

	const auto& lastCommand = m_rotationHistory.back();
	std::stringstream ss;

	if (auto pos = lastCommand.find_first_of('.') != std::string::npos) {
		// rotation was counter-clockwise
		ss << lastCommand[0] << lastCommand.c_str() + pos + 1;
	}
	else {
		// clockwise rotation
		ss << lastCommand[0] << '.' << lastCommand.c_str() + 1;
	}

	Rotate(ss.str(), false);
	m_rotationHistory.pop_back();
}

void RubikCubeControl::SaveRotations(const std::string& filename) const
{
	std::fstream file(filename, std::fstream::out);

	if (!file.good()) {
		throw std::runtime_error("Unable to create file for saving");
	}

	for (const auto& rotationCommand : m_rotationHistory) {
		file << rotationCommand << std::endl;
	}

	file.close();
}

void RubikCubeControl::LoadAndPerformRotations(const std::string& filename)
{
	std::fstream file(filename, std::fstream::in);

	if (!file.good()) {
		throw std::runtime_error("Unable to open savefile");
	}

	while (!file.eof()) {
		std::string rotationCommand;
		file >> rotationCommand;

		if (!rotationCommand.empty()) {
			Rotate(rotationCommand);
			// "hack" - update the cube to finish the rotation animation immediately
			m_rubikCube->Update(1000.f);
		}
	}

	file.close();
}