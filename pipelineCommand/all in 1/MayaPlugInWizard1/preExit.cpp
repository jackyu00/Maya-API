
/*======================================================================
This plugin has an unkonw problem while running on Maya-x64.
The thread of Maya keep staying in system even when the user has exit Maya.
The function of the callback is disconnecting Maya with Alienbrain, so the problerm can be solved.
==========================================================================*/

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSceneMessage.h>
#include <maya/MFileObject.h>
#include <maya/MString.h>
#include <maya/MMessage.h>
#include <maya/MStringArray.h>

#include "preExit.h"

using namespace std;

MCallbackIdArray preExitCmd::preExitCallbackIds;

int isPreExit = 0;

void beforeExitCallback( void* )
{	
	MStatus stat;

	int isAlienbrainLoad = 0;
	MString cmd = MString("abMEL_PluginCheck;");
	stat = MGlobal::executeCommand(cmd, isAlienbrainLoad);

	if(isAlienbrainLoad)
	{
		MString result;
		cmd = MString("about -v");
		stat = MGlobal::executeCommand(cmd, result);
		cmd = MString("match \"[0-9]+\" \"")+result+MString("\"");
		stat = MGlobal::executeCommand(cmd, result);	

		MString AlienbrainPlugin = MString("Alienbrain for Maya")+result;

		int isx64 = 0;
		cmd = MString("about -x64");
		stat = MGlobal::executeCommand(cmd, isx64);
		if(isx64)
			AlienbrainPlugin += MString("_x64");

		MGlobal::displayInfo(MString("print:")+AlienbrainPlugin);

		cmd = MString("unloadPlugin \"")+AlienbrainPlugin+MString("\"");
		stat = MGlobal::executeCommand(cmd);
	}
}



preExitCmd::~preExitCmd() {}

void* preExitCmd::creator()
{
	return new preExitCmd();
}


MStatus preExitCmd::doIt( const MArgList& args )
{
	MStatus 		stat;
    MCallbackId     id;

	// Parse the arguments.
	for ( unsigned i = 0; i < args.length(); i++ )
	{
		if ( MString( "-q" ) == args.asString( i, &stat ) && MS::kSuccess == stat )
		{
			MString comp = args.asString( i+1, &stat );
			if(MS::kSuccess == stat)
			{
				if ( MString( "-enable" ) == comp )
				{
					setResult( isPreExit );
					return MS::kSuccess;
				}				
			}
			else
				return MS::kFailure;
		}		
		else if ( MString( "-enable" ) == args.asString( i, &stat ) && MS::kSuccess == stat )
		{
			int result = args.asInt( ++i, &stat );
			if ( !stat )			
				return MS::kFailure;
			if(result > 0)
				isPreExit = 1;
			else
				isPreExit = 0;		
		}
		else
		{
			MString msg = "Invalid flag of preExit: ";
			msg += args.asString( i );
			displayError( msg );
			return MS::kFailure;
		}			
	}
	
	if(isPreExit)
	{
		if ( preExitCmd::preExitCallbackIds.length() == 0 )
		{	
			id = MSceneMessage::addCallback( MSceneMessage::kMayaExiting, beforeExitCallback, NULL, &stat );
			if (stat)
				stat = preExitCmd::preExitCallbackIds.append( id );
			else
				MGlobal::displayError(MString("Register callback failed :")+stat.errorString());
		}
	}
	else
	{
		for (unsigned int i=0; i< preExitCmd::preExitCallbackIds.length(); i++ )
			MMessage::removeCallback( (MCallbackId)preExitCmd::preExitCallbackIds[i] );
	}

	return stat;
}