#pragma once

enum class Direction_Movement
{
	NONE = 0,
	FORWARD = 1,
	BACKWARD = 2,
	LEFT = 4,
	RIGHT = 8,
	UP = 16,
	DOWN = 32,
	ROLL_LEFT = 64,
	ROLL_RIGHT = 128
};

// Enable Negative operations for Direction_Movement
inline Direction_Movement operator~(Direction_Movement a)
{
	using T = std::underlying_type_t<Direction_Movement>;
	return static_cast<Direction_Movement>(~static_cast<T>(a));
}

// Enable OR operations for Direction_Movement
inline Direction_Movement operator|(Direction_Movement a, Direction_Movement b)
{
	using T = std::underlying_type_t<Direction_Movement>;
	return static_cast<Direction_Movement>(static_cast<T>(a) | static_cast<T>(b));
}

// Enable AND operations for Direction_Movement
inline Direction_Movement operator&(Direction_Movement a, Direction_Movement b)
{
	using T = std::underlying_type_t<Direction_Movement>;
	return static_cast<Direction_Movement>(static_cast<T>(a) & static_cast<T>(b));
}

// Enable Unite operations for Direction_Movement
inline Direction_Movement& operator|=(Direction_Movement& a, Direction_Movement b)
{
	a = a | b;
	return a;
}

// Enable Intersect operations for Direction_Movement
inline Direction_Movement& operator&=(Direction_Movement& a, Direction_Movement b)
{
	a = a & b;
	return a;
}

inline bool HasFlag(Direction_Movement value)
{
	return static_cast<bool>(value);
}

inline bool HasFlag(Direction_Movement value, Direction_Movement flag)
{
	return (value & flag) != Direction_Movement::NONE;
}

class Camera
{
private:
	// -------------------------------------------------------------------------
	// Private members
	// -------------------------------------------------------------------------
	bool _needsUpdate = false;

public:
	// -------------------------------------------------------------------------
	// Properties
	// -------------------------------------------------------------------------

	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 WorldUp;
	glm::vec3 Front;
	glm::vec3 Right;
	glm::vec3 Up;

	// Euler Angles
	float Pitch;
	float Yaw;
	float Roll;

	// Camera options
	float MovementSpeed = 10.0f;
	float MouseSensitivity = 0.1f;
	float ScrollSensitivity = 1.0f;

	// Clipping planes (Zoom works as FOV)
	float Zoom = 45.0f;
	float RenderDistance = 1000.0f;
	float NearPlane = 0.1f;

	bool EnableZoom = true;
	bool InvertY = false;
	bool EnableRoll = true;
	bool ConstrainPitch = true;

public:
	// -------------------------------------------------------------------------
	// Constructor
	// -------------------------------------------------------------------------
	Camera()
		: Camera(glm::vec3(0.0f, 0.0f, 3.0f)) {
	}

	Camera(const glm::vec3& position, const glm::vec3& worldUp = glm::vec3(0.0f, 1.0f, 0.0f), float pitch = 0.0f, float yaw = -90.0f)
		: Position(position), WorldUp(worldUp), Pitch(pitch), Yaw(yaw)
	{
		updateCameraVectors();
	}

	// -------------------------------------------------------------------------
	// Destructor
	// -------------------------------------------------------------------------
	~Camera() = default;

	// -------------------------------------------------------------------------
	// Returns the LookAt view matrix
	// -------------------------------------------------------------------------
	glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// -------------------------------------------------------------------------
	// Returns a perspective projection matrix
	// -------------------------------------------------------------------------
	glm::mat4 getProjectionMatrix(float aspectRatio) const
	{
		return glm::perspective(glm::radians(Zoom), aspectRatio, 0.1f, 1000.0f);
	}

	// -------------------------------------------------------------------------
	// Keyboard input (WASD)
	// -------------------------------------------------------------------------
	void processMovement(Direction_Movement direction, float deltaTime)
	{
		if (direction == Direction_Movement::NONE)
			return;

		float velocity = MovementSpeed * deltaTime;

		if (HasFlag(direction, Direction_Movement::FORWARD))
			Position += Front * velocity;

		if (HasFlag(direction, Direction_Movement::BACKWARD))
			Position -= Front * velocity;

		if (HasFlag(direction, Direction_Movement::LEFT))
			Position -= Right * velocity;

		if (HasFlag(direction, Direction_Movement::RIGHT))
			Position += Right * velocity;

		if (HasFlag(direction, Direction_Movement::UP))
			Position += Up * velocity;

		if (HasFlag(direction, Direction_Movement::DOWN))
			Position -= Up * velocity;

		if (EnableRoll)
		{
			if (HasFlag(direction, Direction_Movement::ROLL_LEFT))
			{
				Roll -= 25.0f * velocity;
				_needsUpdate = true;
			}

			if (HasFlag(direction, Direction_Movement::ROLL_RIGHT))
			{
				Roll += 25.0f * velocity;
				_needsUpdate = true;
			}
		}

		if (_needsUpdate)
		{
			updateCameraVectors();
			_needsUpdate = false;
		}
	}

	// -------------------------------------------------------------------------
	// Mouse movement (rotates camera)
	// -------------------------------------------------------------------------
	void processMouseMovement(float deltaX, float deltaY)
	{
		Yaw += deltaX * MouseSensitivity;
		if (InvertY)
			Pitch -= deltaY * MouseSensitivity;
		else
			Pitch += deltaY * MouseSensitivity;

		// Constrain vertical rotation
		if (ConstrainPitch)
		{
			Pitch = glm::clamp(Pitch, -89.0f, 89.0f);
		}

		updateCameraVectors();
	}

	// -------------------------------------------------------------------------
	// Mouse scroll (changes Zoom - FOV)
	// -------------------------------------------------------------------------
	void processMouseScroll(float deltaScroll)
	{
		if (!EnableZoom)
			return;

		Zoom = glm::clamp(Zoom - ScrollSensitivity * deltaScroll, 1.0f, 90.0f);
	}

private:
	// -------------------------------------------------------------------------
	// Updates Front, Right, Up vectors from the updated yaw/pitch
	// -------------------------------------------------------------------------
	void updateCameraVectors()
	{
		// Calculate the new Front direction vector
		glm::vec3 front =
		{
			glm::cos(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch)),
			glm::sin(glm::radians(Pitch)),
			glm::sin(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch))
		};

		// Recalculate Front, Right and Up vectors
		Front = glm::normalize(front);
		glm::vec3 right = glm::normalize(glm::cross(Front, WorldUp));
		glm::vec3 up = glm::normalize(glm::cross(right, Front));

		// Recalculate RIGHT and UP axis around the FRONT vector (ROLL)
		glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Roll), Front);
		Right = glm::normalize(glm::vec3(rollMatrix * glm::vec4(right, 0.0f)));
		Up = glm::normalize(glm::vec3(rollMatrix * glm::vec4(up, 0.0f)));
	}
};