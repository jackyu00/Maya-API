#ifndef _preLoadReferenceCMD
#define _preLoadReferenceCMD


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



class preLoadReferenceCmd : public MPxCommand
{
public:
					preLoadReferenceCmd() { initial = true; };
	virtual			~preLoadReferenceCmd(); 
	MStatus			doIt( const MArgList& args );
	static void*	creator();
	bool			initial;
	static MCallbackIdArray preLoadReferenceCallbackIds;	
};


#endif
