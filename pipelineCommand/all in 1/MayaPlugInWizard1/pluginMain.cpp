

#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MSceneMessage.h>
#include <maya/MCallbackIdArray.h>
#include <maya/MMessage.h>

#include "preReference.h"
#include "preLoadReference.h"
#include "preNodeAdd.h"
#include "preSave.h"
#include "preOpen.h"
#include "preExit.h"

MStatus initializePlugin( MObject obj )
{
	MFnPlugin plugin( obj, "Jack Yu" );

	plugin.registerCommand( "preReference", preReferenceCmd::creator );// pre-load reference	
	plugin.registerCommand( "preLoadReference", preLoadReferenceCmd::creator );// file-labeling	
	plugin.registerCommand( "preNodeAdd", preNodeAddCmd::creator );// pre-load texture file
	plugin.registerCommand( "preSave", preSaveCmd::creator );// file label store
	plugin.registerCommand( "preOpen", preOpenCmd::creator );// pre-load opening file	
	plugin.registerCommand( "preExit", preExitCmd::creator );

	MString cmd = MString("preNodeAdd -enable 1;preNodeAdd -autoGetLatest 1;preReference -enable 1;preReference -autoGetLatest 1;preOpen -enable 1;preSave -enable 1;preLoadReference -enable 1;preExit -enable 1");		

	MGlobal::executeCommand(cmd);

	return MS::kSuccess;
}

MStatus uninitializePlugin( MObject obj)
{
	for (unsigned int i=0; i<preReferenceCmd::preReferenceCallbackIds.length(); i++ )	
		MMessage::removeCallback( (MCallbackId)preReferenceCmd::preReferenceCallbackIds[i] );

	for (unsigned int i=0; i<preLoadReferenceCmd::preLoadReferenceCallbackIds.length(); i++ )
		MMessage::removeCallback( (MCallbackId)preLoadReferenceCmd::preLoadReferenceCallbackIds[i] );

	for (unsigned int i=0; i<preSaveCmd::preSaveCallbackIds.length(); i++ )
		MMessage::removeCallback( (MCallbackId)preSaveCmd::preSaveCallbackIds[i] );
	
	for (unsigned int i=0; i<preOpenCmd::preOpenCallbackIds.length(); i++ )	
		MMessage::removeCallback( (MCallbackId)preOpenCmd::preOpenCallbackIds[i] );
	
	for (unsigned int i=0; i< preNodeAddCmd::addNodeCallbackIds.length(); i++ )
		MMessage::removeCallback( (MCallbackId)preNodeAddCmd::addNodeCallbackIds[i] );

	for (unsigned int i=0; i< preNodeAddCmd::attributeChangeCallbackIds.length(); i++ )
		MMessage::removeCallback( (MCallbackId)preNodeAddCmd::attributeChangeCallbackIds[i] );	

	for (unsigned int i=0; i< preExitCmd::preExitCallbackIds.length(); i++ )
		MMessage::removeCallback( (MCallbackId)preExitCmd::preExitCallbackIds[i] );	

	MFnPlugin plugin( obj );

	plugin.deregisterCommand( "preReference" );	
	plugin.deregisterCommand( "preLoadReference" );	
	plugin.deregisterCommand( "preNodeAdd" );
	plugin.deregisterCommand( "preSave" );
	plugin.deregisterCommand( "preOpen" );
	plugin.deregisterCommand( "preExit" );	

	return MS::kSuccess;
}