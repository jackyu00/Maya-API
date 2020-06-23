#ifndef _preReferenceCMD
#define _preReferenceCMD


#include <maya/MIOStream.h>
#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MIntArray.h>
#include <maya/MFileIO.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MSceneMessage.h>
#include <maya/MCallbackIdArray.h>
#include <maya/MFileObject.h>
#include <maya/MString.h>
#include <maya/MScriptUtil.h>
#include <maya/MMessage.h>
#include <maya/MStringArray.h>
#include <maya/MFileIO.h>



class preReferenceCmd : public MPxCommand
{
public:
					preReferenceCmd() { initial = true; };
	virtual			~preReferenceCmd(); 
	MStatus			doIt( const MArgList& args );
	static void*	creator();
	bool			initial;
	static MCallbackIdArray preReferenceCallbackIds;	
};


#endif
