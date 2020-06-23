
/*======================================================================
The function of this callback is to store the label information of each referenced file.
All of the dependency files (audio,texture file,maya file) will be set to link to the saving file.
==========================================================================*/

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

#include "preSave.h"
#include "fileInfoParser.h"

using namespace std;

MCallbackIdArray preSaveCmd::preSaveCallbackIds;

int ispreSave = 0;


void beforeSaveCheckCallback( bool* retCode, void* clientData )
{
	//MGlobal::displayInfo(MString("Running preSave procedure..."));

	retCode[0] = true;

	MString cmd;
	MStatus stat;

	MStringArray resultArray;

	int isAlienbrainLoad = 0;
	cmd = MString("abMEL_PluginCheck;");
	stat = MGlobal::executeCommand(cmd, isAlienbrainLoad);

	//Remove all fileInfo except "label"
	cmd = MString("fileInfo -q");
	MGlobal::executeCommand( cmd, resultArray );

	for ( unsigned int i = 0; i < resultArray.length(); i+=2 )
	{
		MString tmp = resultArray[i];
		if( tmp.toUpperCase() != MString("LABEL") )
		{			
			cmd = MString("fileInfo -rm ")+resultArray[i];
			MGlobal::executeCommand( cmd );
		}
	}	
	
	//get all reference file
	resultArray.clear();
	cmd = MString("file -q -r");
	MGlobal::executeCommand( cmd, resultArray );
	for ( unsigned int i = 0; i < resultArray.length(); i++ )
	{
		// If the returning label is empty string, which means the target file has no label info or it is not a Maya Ascii file

		MString str;
		cmd = MString("match \"[^{]*\" \"") + resultArray[i] + MString("\"");
		MGlobal::executeCommand( cmd, str );
		MString labelNum = fileInfoParser( str, "label" );		
		if( labelNum.numChars() != 0 )
		{
			//set label info with reference node name
			cmd = MString("referenceQuery -rfn \"") + resultArray[i] + MString("\"");
			MString rfnNode;
			MGlobal::executeCommand( cmd, rfnNode );			
			cmd = MString("fileInfo ") + rfnNode + MString(" ")+labelNum;
			MGlobal::executeCommand( cmd );
		}
	}
	MGlobal::displayInfo(MString("Reference file labels stored."));

	if(isAlienbrainLoad)
	{
		cmd = MString("abMEL_UpdateDependency");
		MGlobal::executeCommand( cmd );
	}
	MGlobal::displayInfo(MString("Reference file dependency link updated."));	
}





preSaveCmd::~preSaveCmd() {}

void* preSaveCmd::creator()
{
	return new preSaveCmd();
}

MStatus preSaveCmd::doIt( const MArgList& args )
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
					setResult( ispreSave );
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
				ispreSave = 1;
			else
				ispreSave = 0;
		
		}		
		else
		{
			MString msg = "Invalid flag of preSave: ";
			msg += args.asString( i );
			displayError( msg );
			return MS::kFailure;
		}			
	}


	if(ispreSave)
	{
		if ( preSaveCallbackIds.length() == 0 )
		{		
			id = MSceneMessage::addCallback ( MSceneMessage::kBeforeSaveCheck, beforeSaveCheckCallback, NULL, &stat );
			if (stat)
			{
				preSaveCmd::preSaveCallbackIds.append( id );
			}
			else
				MGlobal::displayError(MString("Register callback failed :")+stat.errorString());
		}
	}
	else
	{

		for (unsigned int i=0; i< preSaveCmd::preSaveCallbackIds.length(); i++ )
			MMessage::removeCallback( (MCallbackId)preSaveCmd::preSaveCallbackIds[i] );
	}

	return stat;
}



