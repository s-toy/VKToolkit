#include "pch.h"
#include <windows.h>
#include "WindowCreator.hpp"
#include "Common.hpp"

using namespace hiveVKT;

class Test_CreateWindow : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
	}

	virtual void TearDown() override
	{
		glfwDestroyWindow(m_pWindow);
	}

	//NOTE: 
	void verifyWindowStatus(const SDisplayInfo& vExpectedDisplayInfo)
	{
		ASSERT_TRUE(m_pWindow);

		int Width, Height, PosX, PosY;
		glfwGetWindowSize(m_pWindow, &Width, &Height);
		glfwGetWindowPos(m_pWindow, &PosX, &PosY);
		bool IsFullScreen = (glfwGetWindowMonitor(m_pWindow) == nullptr) ? false : true;

		EXPECT_EQ(Width,		vExpectedDisplayInfo.WindowWidth);
		EXPECT_EQ(Height,		vExpectedDisplayInfo.WindowHeight);
		EXPECT_EQ(PosX,			vExpectedDisplayInfo.WindowPosX);
		EXPECT_EQ(PosY,			vExpectedDisplayInfo.WindowPosY);
		EXPECT_EQ(IsFullScreen, vExpectedDisplayInfo.IsWindowFullScreen);
	}

	hiveVKT::CWindowCreator m_WindowCreator;
	SDisplayInfo m_DisplayInfo;
	GLFWwindow* m_pWindow = nullptr;
};

//*******************************************************************************
//测试点：创建默认窗口，窗口状态应该与默认值一致
TEST_F(Test_CreateWindow, CreateDefaultWindow)
{
	m_pWindow = m_WindowCreator.create(m_DisplayInfo);
	verifyWindowStatus(m_DisplayInfo);
}

//*******************************************************************************
//测试点：自定义窗口大小, 正常情况
TEST_F(Test_CreateWindow, CustomWindowSize_Case1)
{
	m_DisplayInfo.WindowWidth = 500;
	m_DisplayInfo.WindowHeight = 500;
	m_pWindow = m_WindowCreator.create(m_DisplayInfo);
	verifyWindowStatus(m_DisplayInfo);
}

//*******************************************************************************
//测试点：自定义窗口大小, 窗口大小大于屏幕分辨率
TEST_F(Test_CreateWindow, CustomWindowSize_Case2)
{
	m_DisplayInfo.WindowWidth = ;
	m_DisplayInfo.WindowHeight = ;
	m_pWindow = m_WindowCreator.create(m_DisplayInfo);

	SDisplayInfo ExpectedDisplayInfo = m_DisplayInfo;
}

//*******************************************************************************
//TEST POINT: To verify that the CWindowCreator::create function can handle illegal input.
TEST_F(Test_CreateWindow, CreateWindow_InvalidInputParams)
{
	m_DisplayInfo.WindowWidth = -10;
	m_DisplayInfo.WindowHeight = 0;

	m_pWindow = m_WindowCreator.create(m_DisplayInfo);
	ASSERT_TRUE(!m_pWindow);
}

//*******************************************************************************
//TEST POINT: To verity that the CWindowCreator::create function can create a fullscreen window.
TEST_F(Test_CreateWindow, CreateWindow_FullScreen)
{
	m_DisplayInfo.IsWindowFullScreen = true;
	m_pWindow = m_WindowCreator.create(m_DisplayInfo);
	ASSERT_TRUE(m_pWindow);

	EXPECT_TRUE(glfwGetWindowMonitor(m_pWindow));	//NOTE: glfwGetWindowMonitor returns the handle of the monitor that the specified window is in full screen on.

	int Width, Height;
	glfwGetWindowSize(m_pWindow, &Width, &Height);
	EXPECT_EQ(Width, GetSystemMetrics(SM_CXSCREEN));	//NOTE: use GetSystemMetrics to get screen size.
	EXPECT_EQ(Height, GetSystemMetrics(SM_CYSCREEN));
}