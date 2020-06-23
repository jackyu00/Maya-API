
/*======================================================================
The function of this callback is to examine the label information of the referenced file.
First it will get the label nmuber of the current referenced file by querying fileInfo information.
Next it will get the label number of the target file on data base.
If the label number donesnt match, the path of the referenced file will be modified to point to the correct old version (which the file was referenced before).
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

#include "preLoadReference.h"
#include "fileInfoParser.h"

using namespace std;

MCallbackIdArray preLoadReferenceCmd::preLoadReferenceCallbackIds;

int ispreLoadReference = 0;


void beforeLoadReferenceCheckCallback( bool* retCode, MFileObject& file, void* clientData )
{
	//MGlobal::displayInfo(MString("Running preLoadReference procedure..."));

	//MGlobal::displayInfo(file.expandedPath());//	data/prop/egg/ok/
	//MGlobal::displayInfo(file.resolvedPath());//		O:/MuMuHug2009/CGI/element/data/prop/egg/ok/
	//MGlobal::displayInfo(file.resolvedFullName());//		O:/MuMuHug2009/CGI/element/data/prop/egg/ok/egg.ma	
	//MGlobal::displayInfo(file.rawFullName());//		O:/MuMuHug2009/CGI/element/data/prop/egg/ok/egg.ma
	//MGlobal::displayInfo(file.rawPath());//			data/prop/egg/ok/
	//MGlobal::displayInfo(file.rawName());//			egg.ma	

	//set retCode parameter "1" to enable this operation
	retCode[0] = true;

	//if the file type is not *.ma, stop the procudure
	if( file.rawFullName().substring(file.rawFullName().numChars()-2, file.rawFullName().numChars()).toUpperCase() != MString("MA") )
		return;

	int isAlienbrainLoad = 0;
	MString cmd = MString("abMEL_PluginCheck;");
	MStatus stat = MGlobal::executeCommand(cmd, isAlienbrainLoad);

	//*************label comparing****************************************************************************

	cmd = MString("file -q -rfn \"")+file.resolvedFullName()+MString("\"");
	MString result;
	MGlobal::executeCommand( cmd, result );
	MString rfn = result;

	//get label info from current opening file
	cmd = MString("fileInfo -q ")+rfn;
	MStringArray resultArray;
	MGlobal::executeCommand( cmd, resultArray );
	if( resultArray[0].length() == 0 )
		resultArray[0] = MString("0");

	//get label info from referenced file
	result.clear();
	result = fileInfoParser( file.resolvedFullName(), MString("label") );
	if( result.length() == 0 )
		result = MString("0");

	if( result != resultArray[0] ) // if the label doesn't match
	{
		MString fileBaseName, fileDir ,fileExt;
		cmd = MString("basenameEx(\"")+file.rawName()+MString("\")");
		stat = MGlobal::executeCommand(cmd, fileBaseName);

		cmd = MString("fileExtension(\"")+file.rawName()+MString("\")");
		stat = MGlobal::executeCommand(cmd, fileExt);

		MString unresolvedFileName = MString("label/") + fileBaseName + MString(".") + resultArray[0] + MString(".") + fileExt;
		//	label/	+ mumu + . + 0 + . + ma
		
		//---------------------------------------------------------------------------------------------
		MString filePath = file.resolvedPath();

		//get latest version	
		if(isAlienbrainLoad)
		{			
			cmd = MString( "abMEL_GetLatestVersion(\"")+filePath+unresolvedFileName+MString("\")" );
			MGlobal::executeCommand( cmd );		
		}

		//if new file doesn't exist, load the old reference file
		cmd = MString("filetest -r \"")+filePath+unresolvedFileName+MString("\"");
		int res = 0;
		MGlobal::executeCommand( cmd, res );		
		if(res)
		{			
			//replace reference			
			MGlobal::displayInfo( MString("\n// Result: Referenced file \"")+file.rawPath()+file.rawName()+MString("\" was replaced with \"")+file.rawPath()+unresolvedFileName+MString("\" by file-labeling callback.") );	
			file.setRawFullName(file.rawPath()+unresolvedFileName);
		}	
	}
}





preLoadReferenceCmd::~preLoadReferenceCmd() {}

void* preLoadReferenceCmd::creator()
{
	return new preLoadReferenceCmd();
}

MStatus preLoadReferenceCmd::doIt( const MArgList& args )
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
					setResult( ispreLoadReference );
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
				ispreLoadReference = 1;
			else
				ispreLoadReference = 0;
		
		}		
		else
		{
			MString msg = "Invalid flag of preLoadReference: ";
			msg += args.asString( i );
			displayError( msg );
			return MS::kFailure;
		}			
	}

	
	if(ispreLoadReference)
	{
		if ( preLoadReferenceCallbackIds.length() == 0 )
		{	
			id = MSceneMessage::addCheckFileCallback ( MSceneMessage::kBeforeLoadReferenceCheck, beforeLoadReferenceCheckCallback, NULL, &stat );
			if (stat)
			{
				preLoadReferenceCmd::preLoadReferenceCallbackIds.append( id );
			}
			else
				MGlobal::displayError(MString("Register callback failed :")+stat.errorString());
		}
	}
	else
	{

		for (unsigned int i=0; i< preLoadReferenceCmd::preLoadReferenceCallbackIds.length(); i++ )
			MMessage::removeCallback( (MCallbackId)preLoadReferenceCmd::preLoadReferenceCallbackIds[i] );
	}
	
	

	return stat;
}



