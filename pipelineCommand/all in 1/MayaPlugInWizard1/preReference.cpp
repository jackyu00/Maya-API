
/*======================================================================
The function of this callback is to get the referenced maya files automatically.
The flag "autoGetLatest" will determine if the referenced file should be overwrited if it is not the latest version.
edit #20101001 : fix the problem that Alienbrain can't regonize relative path which contain string "/../"; to keep the referenced file path un-chenged after opening, we use overrideResolvedFullName method to set new file path"
==========================================================================*/

#define MAYAVERSION 2008

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSceneMessage.h>
#include <maya/MFileObject.h>
#include <maya/MString.h>
#include <maya/MMessage.h>
#include <maya/MStringArray.h>
#include <maya/MFileIO.h>


#include "preReference.h"

using namespace std;

MCallbackIdArray preReferenceCmd::preReferenceCallbackIds;

bool isAutoGetlatestReference = 0;
int isPreLoad = 0;
preReferenceCmd::~preReferenceCmd() {}

void usbCheck( bool* retCode, MFileObject& file, void* clientData )
{
	//MGlobal::displayInfo(MString("Running preReference procedure..."));

	retCode[0] = true;
	MStatus stat;	
	MString cmd;	

	// since Maya 2009, the function "setResolveMethod" has been added to MFileObject class
	// we must set the MFileResolveMethod as kNone so that we can get correct resolvedFullName if the target referenced file doesn't exist on disk while opening
	#if MAYAVERSION > 2008
		file.setResolveMethod(MFileObject::MFileResolveMethod::kNone);
	#endif

	/*
	MGlobal::displayInfo(MString("rawName : ")+file.rawName());
	MGlobal::displayInfo(MString("rawPath : ")+file.rawPath());
	MGlobal::displayInfo(MString("rawFullName : ")+file.rawFullName());
	MGlobal::displayInfo(MString("expandedPath : ")+file.expandedPath());
	MGlobal::displayInfo(MString("expandedFullName : ")+file.expandedFullName());
	MGlobal::displayInfo(MString("resolvedName : ")+file.resolvedName());
	MGlobal::displayInfo(MString("resolvedPath : ")+file.resolvedPath());
	MGlobal::displayInfo(MString("resolvedFullName : ")+file.resolvedFullName());
	*/

	int isAlienbrainLoad = 0;
	cmd = MString("abMEL_PluginCheck;");
	stat = MGlobal::executeCommand(cmd, isAlienbrainLoad);

	if(isAlienbrainLoad)
	{
		MString newfileName;
		newfileName = file.resolvedFullName();

		// deal with the relative file path which contain ".." strings
		// ex : "O:/Monster_Coin/CGI/ep/e001/../../Element/data/stage/School/ok/school_low.mb" -> "O:/Monster_Coin/CGI///Element/data/stage/School/ok/school_low.mb"
		MStringArray pathArray;
		newfileName.split('/', pathArray);
		for(unsigned int i=0; i<pathArray.length(); i++)
		{
			if(pathArray[i] == "..")
			{
				pathArray[i] = "";
				for(unsigned int j=i-1; j>=0; j--)
				{
					if(pathArray[j] != "" )
					{pathArray[j] = "";break;}
				}
			}
		}
		newfileName = pathArray[0];
		for(unsigned int i=1; i<pathArray.length(); i++)	
			newfileName += MString("/")+pathArray[i];	

		file.overrideResolvedFullName(newfileName);

		//get reference file from alienbrain
		if(isAutoGetlatestReference)
		{
			cmd = MString("abMEL_GetLatestVersion \""+newfileName+"\";");
			stat = MGlobal::executeCommand(cmd);		
		}
		else
		{
			int result = 0;
			cmd = MString("filetest -r \""+newfileName+"\";");
			stat = MGlobal::executeCommand(cmd, result);
			if( result == 0 )
			{
				cmd = MString("abMEL_GetLatestVersion \""+newfileName+"\";");
				stat = MGlobal::executeCommand(cmd);
			}
		}
	}
}

void* preReferenceCmd::creator()
{
	return new preReferenceCmd();
}


MStatus preReferenceCmd::doIt( const MArgList& args )
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
					setResult( isPreLoad );
					return MS::kSuccess;
				}
				else if ( MString( "-autoGetLatest" ) == comp )
				{
					setResult( isAutoGetlatestReference );
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
				isPreLoad = 1;
			else
				isPreLoad = 0;
		
		}
		else if ( MString( "-autoGetLatest" ) == args.asString( i, &stat ) && MS::kSuccess == stat )
		{
			int result = args.asInt( ++i, &stat );
			if ( !stat )			
				return MS::kFailure;
			if(result > 0)
				isAutoGetlatestReference = 1;
			else
				isAutoGetlatestReference = 0;
		}		
		else
		{
			MString msg = "Invalid flag of preReference: ";
			msg += args.asString( i );
			displayError( msg );
			return MS::kFailure;
		}			
	}


	if(isPreLoad)
	{
		if ( preReferenceCallbackIds.length() == 0 )
		{
			id = MSceneMessage::addCheckFileCallback ( MSceneMessage::kBeforeReferenceCheck, usbCheck, NULL, &stat);
			if (stat)
			{
				preReferenceCmd::preReferenceCallbackIds.append( id );
			}
			else
				MGlobal::displayError(MString("Register callback failed :")+stat.errorString());
		}
	}
	else
	{

		for (unsigned int i=0; i< preReferenceCmd::preReferenceCallbackIds.length(); i++ )
			MMessage::removeCallback( (MCallbackId)preReferenceCmd::preReferenceCallbackIds[i] );
	}

	return stat;
}