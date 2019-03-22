#include "pch.h"
#include <windows.h>
#include "WindowCreator.hpp"

using namespace hiveVKT;

class Test_CreateWindow : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		m_DisplayInfo.WindowWidth = DEFAULT_WIN_WIDTH;
		m_DisplayInfo.WindowHeight = DEFAULT_WIN_HEIGHT;
		m_DisplayInfo.WindowPosX = DEFAULT_WIN_POS_X;
		m_DisplayInfo.WindowPosY = DEFAULT_WIN_POS_Y;
	}

	virtual void TearDown() override
	{
		glfwDestroyWindow(m_pWindow);
	}

	SWindowCreateInfo m_DisplayInfo;
	GLFWwindow* m_pWindow = nullptr;
};

//*******************************************************************************
//TEST POINT: To verify that the CWindowCreator::create function can create a window successfully.
TEST_F(Test_CreateWindow, CreateWindow)
{
	hiveVKT::CWindowCreator WindowCreator;

	m_pWindow = WindowCreator.create(m_DisplayInfo);
	ASSERT_TRUE(m_pWindow);

	int Width, Height, PosX, PosY;
	glfwGetWindowSize(m_pWindow, &Width, &Height);
	glfwGetWindowPos(m_pWindow, &PosX, &PosY);

	EXPECT_EQ(Width, m_DisplayInfo.WindowWidth);
	EXPECT_EQ(Height, m_DisplayInfo.WindowHeight);
	EXPECT_EQ(PosX, m_DisplayInfo.WindowPosX);
	EXPECT_EQ(PosY, m_DisplayInfo.WindowPosY);

	EXPECT_TRUE(nullptr == glfwGetWindowMonitor(m_pWindow));	//NOTE: glfwGetWindowMonitor should return NULL in windowed mode.
}

//*******************************************************************************
//TEST POINT: To verify that the CWindowCreator::create function can handle illegal input.
TEST_F(Test_CreateWindow, CreateWindow_InvalidInputParams)
{
	hiveVKT::CWindowCreator WindowCreator;

	m_DisplayInfo.WindowWidth = -10;
	m_DisplayInfo.WindowHeight = 0;

	m_pWindow = WindowCreator.create(m_DisplayInfo);
	ASSERT_TRUE(!m_pWindow);
}

//*******************************************************************************
//TEST POINT: To verity that the CWindowCreator::create function can create a fullscreen window.
TEST_F(Test_CreateWindow, CreateWindow_FullScreen)
{
	m_DisplayInfo.IsWindowFullScreen = true;

	hiveVKT::CWindowCreator WindowCreator;
	m_pWindow = WindowCreator.create(m_DisplayInfo);
	ASSERT_TRUE(m_pWindow);

	EXPECT_TRUE(glfwGetWindowMonitor(m_pWindow));	//NOTE: glfwGetWindowMonitor returns the handle of the monitor that the specified window is in full screen on.

	int Width, Height;
	glfwGetWindowSize(m_pWindow, &Width, &Height);
	EXPECT_EQ(Width, GetSystemMetrics(SM_CXSCREEN));	//NOTE: use GetSystemMetrics to get screen size.
	EXPECT_EQ(Height, GetSystemMetrics(SM_CYSCREEN));
}