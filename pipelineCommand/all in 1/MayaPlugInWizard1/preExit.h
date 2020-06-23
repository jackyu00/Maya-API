


#ifndef _preExit
#define _preExit


#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSceneMessage.h>
#include <maya/MCallbackIdArray.h>
#include <maya/MFileObject.h>
#include <maya/MString.h>
#include <maya/MMessage.h>
#include <maya/MStringArray.h>
#include <maya/MFileIO.h>

 
class preExitCmd : public MPxCommand
{
public:
					preExitCmd() {};
	virtual			~preExitCmd(); 
	MStatus			doIt( const MArgList& args );
	static void*	creator();
	static MCallbackIdArray preExitCallbackIds;	
};

#endif


