


#ifndef _preOpen
#define _preOpen


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

 
class preOpenCmd : public MPxCommand
{
public:
					preOpenCmd() {};
	virtual			~preOpenCmd(); 
	MStatus			doIt( const MArgList& args );
	static void*	creator();
	static MCallbackIdArray preOpenCallbackIds;	
};

#endif


