

#ifndef _FILEINFOPARSER
#define _FILEINFOPARSER

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


#include <fstream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;

static MString fileInfoParser( MString filename, MString varname )
{
	MString ss = MString("fileInfo \"") + varname + MString("\"");
	string str1 (ss.asChar());
	MString result;
	
	ifstream infile;
	infile.open (filename.asChar());

	string str2 = "";

	if (infile.is_open())
	{
		while (getline(infile,str2))
		{			
			if (str1.compare(0,16,str2,0,16) == 0)
			{
				MString tmp = MString(str2.c_str());
				MStringArray strings;
				MStringArray strings2;		
				tmp.split( ' ', strings);//remove white-space symbol
				strings[2].split( '\"', strings2);//remove " symbol
				result = strings2[0];
				break;
			}
		}
		infile.close();		
	}
	return result;
}

#endif