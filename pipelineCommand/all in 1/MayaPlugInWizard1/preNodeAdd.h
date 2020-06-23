


#ifndef _preNodeAddCMD
#define _preNodeAddCMD


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

 
class preNodeAddCmd : public MPxCommand
{
public:
					preNodeAddCmd() {};
	virtual			~preNodeAddCmd(); 
	MStatus			doIt( const MArgList& args );
	static void*	creator();
	static MCallbackIdArray addNodeCallbackIds;	
	static MCallbackIdArray attributeChangeCallbackIds;	
	static MIntArray callbackIndex;	
};


#endif


