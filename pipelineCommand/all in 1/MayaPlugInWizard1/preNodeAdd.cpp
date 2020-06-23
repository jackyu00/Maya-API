
/*======================================================================
The function of this callback is to retrive the texture and audio files from Alienbrain automatically.
The flag "autoGetLatest" will determine if the texture file should be overwrited if it is not the latest version.

2010/3/9 Add fur-attr maps auto pre-load, we de-register all attr-change callback after opening process
==========================================================================*/

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MDGMessage.h>
#include <maya/MPlug.h>
#include <maya/MString.h>
#include <maya/MNodeMessage.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MCallbackIdArray.h>
#include <maya/MIntArray.h>
#include <maya/MSceneMessage.h>
#include "preNodeAdd.h"

using namespace std;

MCallbackIdArray preNodeAddCmd::addNodeCallbackIds;
MCallbackIdArray preNodeAddCmd::attributeChangeCallbackIds;
MIntArray preNodeAddCmd::callbackIndex;

bool isAutoGetlatestNodeFile = 0;
int isPreNodeAdd = 0;

float abVersion_preNode(){
		bool abVersion95 = false;
		MString abVersionS, cmd;
		MStringArray abVersionDigs;

		cmd = MString("abAbout;");
		MGlobal::executeCommand(cmd, abVersionS);
		abVersionS.split('.', abVersionDigs);
		abVersionS = abVersionDigs[0] + "." + abVersionDigs[1];
		return abVersionS.asFloat();
}


void resolvePath(MString &path){
	// remove string "../"
	
	int pos = -1;
	while( (pos = path.indexW("../")) != -1){
		MString upper = path.substringW(0, pos-2);
		MString lower = path.substringW(pos +3, -1);
		upper = upper.substringW(0, upper.rindexW("/"));
		path = upper + lower;
	}
}

preNodeAddCmd::~preNodeAddCmd() {}

void attributeChangeCallback( MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData )
{
	MString cmd;
	MStatus stat;

	//MGlobal::displayInfo( plug.partialName());

	int isAlienbrainLoad = 0;
	cmd = MString("abMEL_PluginCheck;");
	stat = MGlobal::executeCommand(cmd, isAlienbrainLoad);

	/*
	Registered attritube list :
	
	file
		fileTextureName (ftn)
	
	audio
		filename (f)
	
	imagePlane
		imageName (imn)
	
	FurDescription
		BaseAmbientColorMap (bacm)
		SpecularColorMap (scm)
		BaseColorMap (bcm)
		TipColorMap (tcm)
		LengthMap (lm)
		SpecularSharpnessMap (ssm)
		BaldnessMap (bm)
		BaseOpacityMap (bom)
		TipOpacityMap (tom)
		BaseWidthMap (bwm)
		TipWidthMap (twm)
		SegmentsMap (sgm)
		BaseCurlMap (bclm)
		TipCurlMap (tclm)
		ScraggleMap (sm)
		ScraggleFrequencyMap (sfm)
		ScraggleCorrelationMap (scom)
		InclinationMap (im)
		RollMap (rm)
		AttractionMap (atm)
		OffsetMap (ofsm)
		ClumpingMap (cm)
		ClumpingFrequencyMap (cfm)
		ClumpShapeMap (csm)
		PolarMap (pm)
	
	brush
		imageName (imn)
		flowerImage (fim)
		leafImage (lim)
	
	RenderManShader
		ImageFile_File (ImageFile_File)
		DarkTreeShader_1_File (DarkTreeShader_1_File)
		DarkTreeDisplacement (DarkTreeDisplacement)
	
// not yet 
	
	nurbsSurface
		miProxyFile

	mesh
		miProxyFile	
	
	*/
	
	if(isAlienbrainLoad)
	{
		MString attrName = plug.partialName();
		if ( msg & MNodeMessage::kAttributeSet ) 
		{	
			//The change of attribute was detected with short name by callback.
			if( 	attrName == MString("ftn") || 
				attrName == MString("f") || 
				attrName == MString("tcm[0]") || 
				attrName == MString("lm[0]") ||	
				attrName == MString("bacm[0]") || 
				attrName == MString("tacm[0]") || 
				attrName == MString("scm[0]") || 
				attrName == MString("ssm[0]") || 
				attrName == MString("bm[0]") || 
				attrName == MString("bom[0]") || 
				attrName == MString("tom[0]") || 
				attrName == MString("bwm[0]") || 
				attrName == MString("sgm[0]") || 
				attrName == MString("lm[0]") || 
				attrName == MString("bclm[0]") || 
				attrName == MString("tclm[0]") || 
				attrName == MString("sm[0]") || 
				attrName == MString("sfm[0]") || 
				attrName == MString("scom[0]") || 
				attrName == MString("im[0]") || 
				attrName == MString("rm[0]") || 
				attrName == MString("pm[0]") || 
				attrName == MString("atm[0]") || 
				attrName == MString("ofsm[0]") || 
				attrName == MString("cm[0]") || 
				attrName == MString("cfm[0]") || 
				attrName == MString("bcm[0]") || 
				attrName == MString("csm[0]") ||
				attrName == MString("ImageFile_File") ||
				attrName == MString("DarkTreeShader_1_File") ||
				attrName == MString("DarkTreeDisplacement_1_File") ||
				attrName == MString("imn") ||
				attrName == MString("lim") ||
				attrName == MString("fim") )
			{	
				MString fileName;
				plug.getValue(fileName);
				
				/* <<The job of de-registering callback has been replaced by function AfterOpenCallback>>
				//	we must remove the callback before setting the new value to attribute, or we will get into infinity recycle loop
				int *index = (int*)clientData;
				stat = MMessage::removeCallback( preNodeAddCmd::attributeChangeCallbackIds[index[0]] );
				preNodeAddCmd::callbackIndex.remove(index[0]);
				*/

				if( fileName.length() != 0 )
				{
					//MGlobal::displayInfo(fileName);
					/*
					MString	projectPath;	
					cmd = MString("workspace -q -rd");
					stat = MGlobal::executeCommand(cmd, projectPath);
					MString newfileName = projectPath + fileName;	

					// check if project path should be added
					cmd = MString("gmatch \""+newfileName+"\" \"*:*:*\"" );
					int isMatch = 0;
					stat = MGlobal::executeCommand(cmd, isMatch);
					if(isMatch)
						newfileName = fileName;
					*/
					cmd = MString("workspace -q -rd");
					MString workspace;
					MGlobal::executeCommand( cmd, workspace );		
					
					MString newfileName = fileName;

					if (abVersion_preNode() > 9.0 ) {
						cmd = MString("gmatch ");
						cmd += ("\"" + workspace + "\" \"" + fileName + "\";") ;
						MString comRes;
						MGlobal::executeCommand( cmd, comRes );		

						if ( comRes =="1" ) {
							cmd = MString("substitute \"(") + workspace + MString(")+\" \"") +  workspace + fileName + MString("\" \"") + workspace + MString("\"");
							MGlobal::executeCommand( cmd, newfileName );					
							resolvePath(newfileName);	
						}

					} else {	
							cmd = MString("substitute \"(") + workspace + MString(")+\" \"") +  workspace + fileName + MString("\" \"") + workspace + MString("\"");
							MGlobal::executeCommand( cmd, newfileName );					
							resolvePath(newfileName);	
					}
					
					if(isAutoGetlatestNodeFile)
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
		}
	}
}


void addNodeCallback( MObject& node, void *clientData )
{
	MStatus 		stat;
	//	When Maya initialize the node, the value of attribute hasn't been set yet
	//	in order to get value and modify it, we have to register another callback with "addAttributeChangedCallback"

	//	There will be lots of texture file nodes need to be registered addAttributeChangedCallback callback 
	//	we have to make sure each specific callback will be remove in the end of callback function
	//	so here we used MIntArray to store the callback index for current file node, and passed by client data pointer in callback function

	preNodeAddCmd::callbackIndex.append(preNodeAddCmd::attributeChangeCallbackIds.length());
	MCallbackId id = MNodeMessage:: addAttributeChangedCallback(node, attributeChangeCallback, &preNodeAddCmd::callbackIndex[preNodeAddCmd::callbackIndex.length()-1], &stat);
	preNodeAddCmd::attributeChangeCallbackIds.append(id);
	
}

void* preNodeAddCmd::creator()
{
	return new preNodeAddCmd();
}

void AfterOpenCallback( void* clientData )
{	
	for (unsigned int i=0; i< preNodeAddCmd::attributeChangeCallbackIds.length(); i++ )
		MMessage::removeCallback( (MCallbackId)preNodeAddCmd::attributeChangeCallbackIds[i] );
}


MStatus preNodeAddCmd::doIt( const MArgList& args )
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
					setResult( isPreNodeAdd );
					return MS::kSuccess;
				}
				else if ( MString( "-autoGetLatest" ) == comp )
				{
					setResult( isAutoGetlatestNodeFile );
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
				isPreNodeAdd = 1;
			else
				isPreNodeAdd = 0;
		
		}
		else if ( MString( "-autoGetLatest" ) == args.asString( i, &stat ) && MS::kSuccess == stat )
		{
			int result = args.asInt( ++i, &stat );
			if ( !stat )			
				return MS::kFailure;
			if(result > 0)
				isAutoGetlatestNodeFile = 1;
			else
				isAutoGetlatestNodeFile = 0;
		}		
		else
		{
			MString msg = "Invalid flag of preNodeAdd: ";
			msg += args.asString( i );
			displayError( msg );
			return MS::kFailure;
		}			
	}

	if(isPreNodeAdd)
	{
		if ( addNodeCallbackIds.length() == 0 )
		{		
			//register textrue file node add callback
			id = MDGMessage::addNodeAddedCallback ( addNodeCallback, MString("file") ,NULL, &stat );
			if (stat)			
				preNodeAddCmd::addNodeCallbackIds.append( id );			
			else
				MGlobal::displayError(MString("Register addNode(file) callback failed :")+stat.errorString());

			// Register texture file which associated with brush node				
			id = MDGMessage::addNodeAddedCallback ( addNodeCallback, MString("brush") ,NULL, &stat );
			if (stat)			
				preNodeAddCmd::addNodeCallbackIds.append( id );
			else
				MGlobal::displayError(MString("Register addNode(brush) callback failed :")+stat.errorString());	

			//register audio file node add callback
			id = MDGMessage::addNodeAddedCallback ( addNodeCallback, MString("audio") ,NULL, &stat );
			if (stat)			
				preNodeAddCmd::addNodeCallbackIds.append( id );			
			else
				MGlobal::displayError(MString("Register addNode(audio) callback failed :")+stat.errorString());

			// Register FurDescription file node add callback, if fur plugin not loaded at this moment, the callback can't be registered
			MString cmd = MString("pluginInfo -q -loaded Fur;");
			int isFurLoad = 0;
			stat = MGlobal::executeCommand(cmd, isFurLoad);
			if(isFurLoad) 
			{				
				id = MDGMessage::addNodeAddedCallback ( addNodeCallback, MString("FurDescription") ,NULL, &stat );
				if (stat)			
					preNodeAddCmd::addNodeCallbackIds.append( id );
				else
					MGlobal::displayError(MString("Register addNode(FurDescription) callback failed :")+stat.errorString());
			}		

			// Register RMS shader (surface,displacement,volume) node add callback, if RenderMan_for_Mya plugin not loaded at this moment, the callback can't be registered
			cmd = MString("pluginInfo -q -loaded RenderMan_for_Maya;");
			int isRMSLoad = 0;
			stat = MGlobal::executeCommand(cmd, isRMSLoad);
			if(isRMSLoad) 
			{				
				id = MDGMessage::addNodeAddedCallback ( addNodeCallback, MString("RenderManShader") ,NULL, &stat );
				if (stat)		
					preNodeAddCmd::addNodeCallbackIds.append( id );
				else
					MGlobal::displayError(MString("Register addNode(RenderManShader) callback failed :")+stat.errorString());

				id = MDGMessage::addNodeAddedCallback ( addNodeCallback, MString("RenderManDisplacement") ,NULL, &stat );
				if (stat)			
					preNodeAddCmd::addNodeCallbackIds.append( id );
				else
					MGlobal::displayError(MString("Register addNode(RenderManDisplacement) callback failed :")+stat.errorString());

				id = MDGMessage::addNodeAddedCallback ( addNodeCallback, MString("RenderManVolume") ,NULL, &stat );
				if (stat)			
					preNodeAddCmd::addNodeCallbackIds.append( id );
				else
					MGlobal::displayError(MString("Register addNode(RenderManVolume) callback failed :")+stat.errorString());
			}
		}
	}
	else
	{
		for (unsigned int i=0; i< preNodeAddCmd::addNodeCallbackIds.length(); i++ )
			MMessage::removeCallback( (MCallbackId)preNodeAddCmd::addNodeCallbackIds[i] );

		for (unsigned int i=0; i< preNodeAddCmd::attributeChangeCallbackIds.length(); i++ )
			MMessage::removeCallback( (MCallbackId)preNodeAddCmd::attributeChangeCallbackIds[i] );
	}

	//after opening, we should de-register all attrChange callback in order to work correctly
	MSceneMessage::addCallback ( MSceneMessage::kAfterOpen, AfterOpenCallback, NULL, &stat );
	if (!stat)
		MGlobal::displayError(MString("Register afterOpen callback failed :")+stat.errorString());	

	return stat;
}

