#pragma once
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

namespace hiveVKT
{
	class CCamera
	{
	public:
		CCamera(glm::dvec3 vCameraPos = glm::dvec3(0.0, 0.0, 0.0), float vYaw = -90.0, float vPitch = 0.0, glm::dvec3 vWorldUp = glm::dvec3(0.0, 1.0, 0.0));
		~CCamera();

		void init();
		void update();

		glm::mat4	getViewMatrix() const;
		glm::mat4	getProjectionMatrix() const;
		glm::dvec3	getCameraPos() const { return m_CameraPos; }
		double		getCameraFov() const { return m_Fovy; }

		void setCameraPos(glm::dvec3 vCameraPos) { m_CameraPos = vCameraPos; }
		void setFarPlane(double vFarPlane) { m_Far = vFarPlane; }
		void setMoveSpeed(double vMoveSpeed) { m_MoveSpeed = vMoveSpeed; }
		void setFov(double vFov) { m_Fovy = vFov; }
		void setEnableCursor(bool vIsEnableCursor) { m_IsEnableCursor = vIsEnableCursor; }

	private:
		void __cursorCallback(double vPosX, double vPosY);
		void __mouseScrollCallback(double vOffsetX, double vOffsetY);
		void __mouseButtonCallback(int vButton, int vAction, int vMods);

		void __updateCameraVectors();
		void __processKeyboard();

		glm::dvec3 m_CameraPos;
		glm::dvec3 m_CameraUp;
		glm::dvec3 m_CameraFront;
		glm::dvec3 m_CameraRight;
		glm::dvec3 m_WorldUp;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;

		double m_Pitch = 0.0;
		double m_Yaw = 0.0;
		double m_Fovy = 45.0;
		double m_MoveSpeed = 5.0;
		double m_Sensitivity = 0.03;
		double m_Near = 0.1;
		double m_Far = 100.0;
		bool m_IsEnableCursor = true;
	};
}