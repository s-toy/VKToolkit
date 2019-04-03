#include "Camera.hpp"
#include <iostream>
#include "InputManager.hpp"
#include "Common.hpp"

using namespace hiveVKT;

const float MOUSE_SENSITIVTY = 0.08f;
const float SCROLL_SENSITIVTY = 0.2f;

CCamera::CCamera(glm::dvec3 vCameraPos, float vYaw, float vPitch, glm::dvec3 vWorldUp) :m_CameraPos(vCameraPos), m_Yaw(vYaw), m_Pitch(vPitch), m_WorldUp(vWorldUp)
{
	__updateCameraVectors();
}

CCamera::~CCamera()
{
}

//************************************************************************************
//Function:
void CCamera::init()
{
	CInputManager::getInstance()->registerCursorCallbackFunc(_CALLBACK_2(CCamera::__cursorCallback, this));
	CInputManager::getInstance()->registerScrollCallbackFunc(_CALLBACK_2(CCamera::__mouseScrollCallback, this));
	CInputManager::getInstance()->registerMouseButtonCallbackFunc(_CALLBACK_3(CCamera::__mouseButtonCallback, this));
}

//************************************************************************************
//Function:
void CCamera::__cursorCallback(double vPosX, double vPosY)
{
	if (m_IsEnableCursor)
	{
		std::array<double, 2> CursorOffset = CInputManager::getInstance()->getCursorOffset();
		CursorOffset[0] *= m_Sensitivity;
		CursorOffset[1] *= m_Sensitivity;
		m_Yaw += glm::radians(CursorOffset[0]);
		m_Pitch += glm::radians(CursorOffset[1]);
		if (m_Pitch > glm::radians(89.0))
			m_Pitch = glm::radians(89.0);
		else if (m_Pitch < glm::radians(-89.0))
			m_Pitch = glm::radians(-89.0);
		m_CameraFront.x = cos(m_Pitch) * cos(m_Yaw);
		m_CameraFront.y = sin(m_Pitch);
		m_CameraFront.z = cos(m_Pitch) * sin(m_Yaw);
		m_CameraFront = glm::normalize(m_CameraFront);
		m_CameraRight = normalize(cross(m_CameraUp, -m_CameraFront));
	}
}

//************************************************************************************
//Function:
void CCamera::__mouseScrollCallback(double vOffsetX, double vOffsetY)
{
	if (m_Fovy >= 1.0 && m_Fovy <= 45.0) m_Fovy -= vOffsetY * SCROLL_SENSITIVTY;

	if (m_Fovy < 1.0) { m_Fovy = 1.0; }
	else if (m_Fovy > 45.0) { m_Fovy = 45.0; }
}

//************************************************************************************
//Function:
void CCamera::__mouseButtonCallback(int vButton, int vAction, int vMods)
{
	if (vButton == GLFW_MOUSE_BUTTON_RIGHT && vAction == GLFW_PRESS)
		std::cout << m_CameraPos.x << " " << m_CameraPos.y << " " << m_CameraPos.z << std::endl;
}

//************************************************************************************
//Function:
glm::mat4 CCamera::getViewMatrix() const
{
	return glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
}

//************************************************************************************
//Function:
glm::mat4 CCamera::getProjectionMatrix() const
{
	return glm::perspective(glm::radians(m_Fovy), 1.0, m_Near, m_Far); //TODO: aspect
}

//************************************************************************************
//Function:
void CCamera::update()
{
	__processKeyboard();
}

//************************************************************************************
//Function:
void CCamera::__updateCameraVectors()
{
	glm::vec3 Front;
	Front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	Front.y = sin(glm::radians(m_Pitch));
	Front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

	m_CameraFront = glm::normalize(Front);
	m_CameraRight = glm::normalize(glm::cross(m_CameraFront, m_WorldUp));
	m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_CameraFront));
}

//Function:
void CCamera::__processKeyboard()
{
	_HIVE_SIMPLE_IF(CInputManager::getInstance()->getKeyStatus()[GLFW_KEY_W], m_CameraPos += m_MoveSpeed * m_CameraFront);
	_HIVE_SIMPLE_IF(CInputManager::getInstance()->getKeyStatus()[GLFW_KEY_S], m_CameraPos -= m_MoveSpeed * m_CameraFront);
	_HIVE_SIMPLE_IF(CInputManager::getInstance()->getKeyStatus()[GLFW_KEY_D], m_CameraPos += m_MoveSpeed * m_CameraRight);
	_HIVE_SIMPLE_IF(CInputManager::getInstance()->getKeyStatus()[GLFW_KEY_A], m_CameraPos -= m_MoveSpeed * m_CameraRight);
	_HIVE_SIMPLE_IF(CInputManager::getInstance()->getKeyStatus()[GLFW_KEY_Q], m_CameraPos += m_MoveSpeed * m_CameraUp);
	_HIVE_SIMPLE_IF(CInputManager::getInstance()->getKeyStatus()[GLFW_KEY_E], m_CameraPos -= m_MoveSpeed * m_CameraUp);
}