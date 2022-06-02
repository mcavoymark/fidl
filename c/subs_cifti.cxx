/* Copyright 3/18/15 Washington University.  All Rights Reserved.
   subs_cifti.cxx  $Revision: 1.8 $*/
#include "CiftiException.h"
#include "CiftiFile.h"
#include "MultiDimIterator.h"
#include <cstdio>
#include <iostream>
#include <vector>
//#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include "subs_cifti.h"
using namespace std;
using namespace cifti;
int cifti_getstack(char *file,float *stack) 
{
    try
    {
        CiftiFile inputFile(file);//on-disk reading by default
        const vector<int64_t>& dims = inputFile.getDimensions();
        for (MultiDimIterator<int64_t> iter(vector<int64_t>(dims.begin() + 1, dims.end())); !iter.atEnd(); ++iter)
        {//helper class to iterate over 2D and 3D cifti with the same code - the "+ 1" is to drop the first dimension (row length)
            inputFile.getRow(stack, *iter);
            stack+=dims[0];
        }
    #if 0
    } catch (CiftiException& e) {
        cout << "Caught CiftiException: " + e.whatString().toStdString() << endl;
        return 0;
    }
    #endif
    #if 1
    } catch (CiftiException& e) {
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 0;
    }
    #endif

    return 1;
}
XmlS *cifti_getxml(char *file)
{
    XmlS *xmls;
    vector<char> myData;
    try
    {
        CiftiFile inputFile(file);//on-disk reading by default
        const CiftiXML& myXML = inputFile.getCiftiXML();
        myData = myXML.writeXMLToVector();
    } catch (CiftiException& e) {
        cout << "Caught CiftiException: " + e.whatString().toStdString() << endl;
        return NULL;
    }
    if(!(xmls=(XmlS*)malloc(sizeof*xmls))) {
        cout << "fidlError: Unable to malloc xmls in cifti_getxml " << endl;
        return NULL;
        }
    xmls->size=myData.size();
    if(!(xmls->data=(char*)malloc(sizeof*xmls->data*xmls->size))) {
        cout << "fidlError: Unable to malloc xmls->data in cifti_getxml " << endl;
        return NULL;
        }
    for(size_t i=0;i<xmls->size;i++) xmls->data[i] = myData[i];
    return xmls;
}


int cifti_writedscalar(char *file,char *cifti_xmldata,unsigned int cifti_xmlsize,float *stack)
{
    try
    {
        vector<char> xmlBytes(cifti_xmldata,cifti_xmldata+cifti_xmlsize);
        CiftiXML newXml;
        newXml.readXML(xmlBytes);
        CiftiScalarsMap newMap;
        newMap.setLength(1);
        newXml.setMap(CiftiXML::ALONG_ROW,newMap);
        CiftiFile outputFile;
        outputFile.setWritingFile(file);
        outputFile.setCiftiXML(newXml);
        outputFile.setColumn(stack,0);
        outputFile.writeFile(file);

    #if 0
    } catch (CiftiException& e) {
        cout << "Caught CiftiException: " + e.whatString().toStdString() << endl;
        return 0;
    }
    #endif
    } catch (CiftiException& e) {
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 0;
    }

    return 1;
}
int cifti_writedtseries(char *file,char *cifti_xmldata,unsigned int cifti_xmlsize,float *stack,int64_t tdim,int64_t vol){
    try{
        vector<char> xmlBytes(cifti_xmldata,cifti_xmldata+cifti_xmlsize);
        CiftiXML newXml;
        newXml.readXML(xmlBytes);
        CiftiSeriesMap newMap;
        newMap.setLength(tdim);
        newXml.setMap(CiftiXML::ALONG_ROW,newMap);
        CiftiFile outputFile;
        outputFile.setWritingFile(file);
        outputFile.setCiftiXML(newXml);
        for(int64_t i=0;i<tdim;++i,stack+=vol)outputFile.setColumn(stack,i); 
        outputFile.writeFile(file);
        }
    catch(CiftiException& e){
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 0;
        }
    return 1;
    }
void* cifti_setdscalar(char *file,char *cifti_xmldata,unsigned int cifti_xmlsize){
    void* ret=NULL;
    CiftiFile* realPtr;
    try{
        vector<char> xmlBytes(cifti_xmldata,cifti_xmldata+cifti_xmlsize);
        CiftiXML newXml;
        newXml.readXML(xmlBytes);
        CiftiScalarsMap newMap;
        newMap.setLength(1);
        newXml.setMap(CiftiXML::ALONG_ROW,newMap);
        ret=new CiftiFile();
        realPtr=(CiftiFile*)ret;
        realPtr->setWritingFile(file);
        realPtr->setCiftiXML(newXml);
        } 
    catch(CiftiException& e){
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        if(ret)delete (CiftiFile*)ret;
        return NULL;
        }
    return ret;
    }
void* cifti_setdtseries(char *file,char *cifti_xmldata,unsigned int cifti_xmlsize,int64_t tdim){
    void* ret=NULL;
    CiftiFile* realPtr;
    try{
        vector<char> xmlBytes(cifti_xmldata,cifti_xmldata+cifti_xmlsize);
        CiftiXML newXml;
        newXml.readXML(xmlBytes);
        CiftiSeriesMap newMap;
        newMap.setLength(tdim);
        newXml.setMap(CiftiXML::ALONG_ROW,newMap);
        ret=new CiftiFile();
        realPtr=(CiftiFile*)ret;
        realPtr->setWritingFile(file);
        realPtr->setCiftiXML(newXml);
        }
    catch(CiftiException& e){
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        if(ret)delete (CiftiFile*)ret;
        return NULL;
        }
    return ret;
    }

#if 0
CANNOT DO THIS WITH CIFTI. SEE cifti_getColumn.cxx.
int cifti_setColumn(void* ciftiPtr,int64_t c,float *stack){ 
    CiftiFile* realPtr=(CiftiFile*)ciftiPtr;
    try{
        realPtr->setColumn(stack,c);
        }
    catch(CiftiException& e){
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        if(realPtr)delete realPtr;
        return 0;
        }
    return 1;
    }
int cifti_free(char *file,void* ciftiPtr){
    CiftiFile* realPtr=(CiftiFile*)ciftiPtr;
    try{
        realPtr->writeFile(file);
        }
    catch(CiftiException& e){
        cerr << "Caught CiftiException: " + AString_to_std_string(e.whatString()) << endl;
        return 0;
        }
    if(realPtr)delete realPtr;
    return 1;
    }
#endif
