
/*======================================================================
The function of this callback is to examine the check out status of the opening file.
A prompt window will pop up if the file hasnt been checked out.
Otherwise, if the file donesnt exist on local drive, get the latest version from Alienbain.
==========================================================================*/

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSceneMessage.h>
#include <maya/MFileObject.h>
#include <maya/MString.h>
#include <maya/MMessage.h>
#include <maya/MStringArray.h>

#include "preOpen.h"
#define MAYAVERSION 2008

using namespace std;

MCallbackIdArray preOpenCmd::preOpenCallbackIds;

int isPreOpen = 0;

float abVersion_preOpen(){
		bool abVersion95 = false;
		MString abVersionS, cmd;
		MStringArray abVersionDigs;

		cmd = MString("abAbout;");
		MGlobal::executeCommand(cmd, abVersionS);
		abVersionS.split('.', abVersionDigs);
		abVersionS = abVersionDigs[0] + "." + abVersionDigs[1];
		return abVersionS.asFloat();
}


void beforeOpenCallback( bool* retCode, MFileObject& file, void* clientData )
{	
	//MGlobal::displayInfo(MString("Running pre-open procedure..."));

	MStatus stat;	
	MString cmd;
	retCode[0] = true;
	MString fileFullPath = file.fullName();

	int isAlienbrainLoad = 0;
	cmd = MString("abMEL_PluginCheck;");
	stat = MGlobal::executeCommand(cmd, isAlienbrainLoad);

	/*
	MGlobal::displayInfo(MString("expandedFullName : ")+file.expandedFullName());
	MGlobal::displayInfo(MString("fullName : ")+file.fullName());
	MGlobal::displayInfo(MString("rawName : ")+file.rawName());
	MGlobal::displayInfo(MString("rawFullName : ")+file.rawFullName());
	MGlobal::displayInfo(MString("resolvedName : ")+file.resolvedName());
	MGlobal::displayInfo(MString("resolvedFullName : ")+file.resolvedFullName());
	*/

	if(isAlienbrainLoad)
	{	
		int isFromServer = 0;

 		if (abVersion_preOpen() > 9.0 )
			fileFullPath = "alienbrain://" + fileFullPath.substring(2, fileFullPath.length());
		

		cmd = MString("abMEL_IsExistOnServer \""+fileFullPath+"\";");
		stat = MGlobal::executeCommand(cmd, isFromServer);
			
		if ( isFromServer )	// see if the file exist on server or just exist on local
		{
			int checkOutStat;
			cmd = MString("abMEL_CheckOutState \""+fileFullPath+"\";");
			stat = MGlobal::executeCommand(cmd, checkOutStat);
			if( checkOutStat != 1 )
			{
				//check out the file ?
				int ret = -1;
				cmd = MString("abMEL_CheckOutPrompt;");
				stat = MGlobal::executeCommand(cmd, ret);
				if(ret == -1)
				{
					retCode[0] = false;	
					return;
				}
				int result;
				if(ret == 1)
				{
					cmd = MString("abMEL_CheckOut 2 \""+fileFullPath+"\";");
					stat = MGlobal::executeCommand(cmd, result);
					if( result != 1 )
						MGlobal::displayError(MString("Check out file "+fileFullPath+" failed !"));
				}
			
			}//endof checkout stat !=1		
		
		}//endof isFromServer
		
		int result = 0;

		if (abVersion_preOpen() > 9.0 ) {
			MString fileFullPath2 = file.fullName();
			cmd = MString("abMEL_GetLatestVersion \""+fileFullPath2+"\";");
		} else {
			cmd = MString("abMEL_GetLatestVersion \""+fileFullPath+"\";");
		}
		stat = MGlobal::executeCommand(cmd, result);
	
	}//endof isAlienbrainLoad

}//endof beforeOpenCallback



preOpenCmd::~preOpenCmd() {}

void* preOpenCmd::creator()
{
	return new preOpenCmd();
}


MStatus preOpenCmd::doIt( const MArgList& args )
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
					setResult( isPreOpen );
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
				isPreOpen = 1;
			else
				isPreOpen = 0;		
		}
		else
		{
			MString msg = "Invalid flag of preOpen: ";
			msg += args.asString( i );
			displayError( msg );
			return MS::kFailure;
		}			
	}

	if(isPreOpen)
	{
		if ( preOpenCmd::preOpenCallbackIds.length() == 0 )
		{		
			id = MSceneMessage::addCheckFileCallback ( MSceneMessage::kBeforeOpenCheck, beforeOpenCallback, NULL, &stat );
			if (stat)
				stat = preOpenCmd::preOpenCallbackIds.append( id );
			else
				MGlobal::displayError(MString("Register callback failed :")+stat.errorString());
		}
	}
	else
	{
		for (unsigned int i=0; i< preOpenCmd::preOpenCallbackIds.length(); i++ )
			MMessage::removeCallback( (MCallbackId)preOpenCmd::preOpenCallbackIds[i] );
	}

	return stat;
}