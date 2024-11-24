////////////////////////////////////////////////////
// // Template code for  CSC 473
////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <shared/defs.h>

#include "shared/opengl.h"

#include <string.h>
#include <util/util.h>
#include <GLModel/GLModel.h>
#include "anim.h"
#include "animTcl.h"
#include "myScene.h"
#include "Bezier.h"
#include "GlobalResourceManager.h" // Assumed resource manager header
#include "animTcl.h"
#include "HermiteSpline.h"
#include "SampleParticle.h"
#include "HermiteSimulator.h"
#include "global.h"


// #include "SampleParticle.h"
// #include "HermiteSimulator.h"

//#include <util/jama/tnt_stopwatch.h>
//#include <util/jama/jama_lu.h>

// register a sample variable with the shell.
// Available types are:
// - TCL_LINK_INT 
// - TCL_LINK_FLOAT

int g_testVariable = 10;
int currentPart = 0;


SETVAR myScriptVariables[] = {
	"testVariable", TCL_LINK_INT, (char *) &g_testVariable,
	"",0,(char *) NULL
};

//---------------------------------------------------------------------------------
//			Hooks that are called at appropriate places within anim.cpp
//---------------------------------------------------------------------------------

//// start or end interaction
//void myMouse(int button, int state, int x, int y)
//{
//
//	// let the global resource manager know about the new state of the mouse 
//	// button
//	GlobalResourceManager::use()->setMouseButtonInfo( button, state );
//
//	if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
//	{
//		animTcl::OutputMessage(
//			"My mouse received a mouse button press event\n");
//
//	}
//	if( button == GLUT_LEFT_BUTTON && state == GLUT_UP )
//	{
//		animTcl::OutputMessage(
//			"My mouse received a mouse button release event\n") ;
//	}
//}	// myMouse

void myMouse(int button, int state, int x, int y)
{
	// let the global resource manager know about the new state of the mouse button
	GlobalResourceManager::use()->setMouseButtonInfo(button, state);

	// 当鼠标左键被按下时，启动小车运动
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// 获取已经注册的 SampleParticle 对象
		SampleParticle* porsche = dynamic_cast<SampleParticle*>(GlobalResourceManager::use()->getSystem("porsche"));

		// 确认获取到 SampleParticle 对象
		if (porsche != nullptr) {
			// 调用 SampleParticle 的命令来启动模拟器
			porsche->command(1, new char* { "start" });

			animTcl::OutputMessage("Mouse clicked, starting car movement along HermiteSpline.");
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		animTcl::OutputMessage("Mouse button released.\n");
	}
}

// interaction (mouse motion)
void myMotion(int x, int y)
{

	GLMouseButtonInfo updatedMouseButtonInfo = 
		GlobalResourceManager::use()->getMouseButtonInfo();

	if( updatedMouseButtonInfo.button == GLUT_LEFT_BUTTON )
	{
		animTcl::OutputMessage(
			"My mouse motion callback received a mousemotion event\n") ;
	}

}	// myMotion

void MakePart1(void) {
	bool success;

	// Part 1: Create Hermite Spline System
	HermiteSpline* hermiteSpline = new HermiteSpline("hermite");
	success = GlobalResourceManager::use()->addSystem(hermiteSpline, true);
	assert(success);


	BaseSystem* sampleSystemRetrieval;
	// retrieve the system
	sampleSystemRetrieval =
		GlobalResourceManager::use()->getSystem("hermite");
	// make sure you got it
	assert(sampleSystemRetrieval);
}

void MakePart2(void) {
	bool success;

	// Part 2: Create Hermite Spline System
	HermiteSpline* objectpath = new HermiteSpline("objectpath");
	success = GlobalResourceManager::use()->addSystem(objectpath, true);
	assert(success);

	
	// create SampleParticle
	SampleParticle* porsche = new SampleParticle("porsche");
	porsche->loadModel("data/missile.obj"); 

	success = GlobalResourceManager::use()->addSystem(porsche, true);
	assert(success);
	// porsche->setPath(objectpath);
	/*

	// create HermiteSimulator 模拟器，控制 SampleParticle 沿 HermiteSpline 移动
	HermiteSimulator* hermiteSimulator = new HermiteSimulator("hermiteSim", porsche, objectpath);
	success = GlobalResourceManager::use()->addSimulator(hermiteSimulator, true);
	assert(success);

	*/
	/*
	// 验证系统和模拟器的注册
	BaseSystem * retrievedSystem = GlobalResourceManager::use()->getSystem("objectpath");
	assert(retrievedSystem);

	retrievedSystem = GlobalResourceManager::use()->getSystem("porsche");
	assert(retrievedSystem);

	BaseSimulator* retrievedSimulator = GlobalResourceManager::use()->getSimulator("hermiteSim");
	assert(retrievedSimulator);
	*/
}

void MakeScene(void) {
//	MakePart1();

//	MakePart2();	
}

// OpenGL initialization
void myOpenGLInit(void)
{
	animTcl::OutputMessage("Initialization routine was called.");

}	// myOpenGLInit

void myIdleCB(void)
{
	return;
}	// myIdleCB

//void myKey(unsigned char key, int x, int y)
//{
//	 animTcl::OutputMessage("My key callback received a key press event\n");
//	return;
//}	// myKey

void myKey(unsigned char key, int x, int y) {
	if (key == 'r') {
		SampleParticle* porsche = dynamic_cast<SampleParticle*>(GlobalResourceManager::use()->getSystem("porsche"));
		if (porsche != nullptr) {
			porsche->command(1, new char* { "reset" });
		}
	}
	else if (key == 's') {
		SampleParticle* porsche = dynamic_cast<SampleParticle*>(GlobalResourceManager::use()->getSystem("porsche"));
		if (porsche != nullptr) {
			porsche->command(1, new char* { "start" });
		}
	}
}

static int testGlobalCommand(ClientData clientData, Tcl_Interp *interp, int argc, myCONST_SPEC char **argv)
{
	 animTcl::OutputMessage("This is a test command!");
    animTcl::OutputResult("100") ;
	return TCL_OK;

}	// testGlobalCommand


void mySetScriptCommands(Tcl_Interp *interp)
{

	// here you can register additional generic (they do not belong to any object) 
	// commands with the shell

	Tcl_CreateCommand(interp, "test", testGlobalCommand, (ClientData) NULL,
					  (Tcl_CmdDeleteProc *)	NULL);

	Tcl_CreateCommand(interp, "part1",
		[](ClientData clientData, Tcl_Interp* interp, int argc, myCONST_SPEC char** argv) {
			currentPart = 1;
			MakePart1();

			animTcl::OutputMessage("Started part 1");
		
			return TCL_OK;
		}, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

	Tcl_CreateCommand(interp, "part2",
		[](ClientData clientData, Tcl_Interp* interp, int argc, myCONST_SPEC char** argv) {
			currentPart = 2;
			MakePart2();

			animTcl::OutputMessage("Started part 2");

			return TCL_OK;
		}, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
}