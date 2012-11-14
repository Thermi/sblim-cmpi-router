/**
 * @file
 * cmpiOSBase_NextHopIPRoute.c
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Eclipse Public License from
 * http://www.opensource.org/licenses/eclipse-1.0.php
 *
 * @author       Federico Martin Casares (warptrosse@gmail.com)
 *
 * @contributors
 *
 * @description
 * This is the factory implementation for creating instances of CIM
 * class Linux_NextHopIPRoute.
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpimacs.h"
#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"
#include "cmpiOSBase_NextHopIPRoute.h"
#include "OSBase_NextHopIPRoute.h"


////////////////////////////////////////////////////////////////////////////////
// FACTORY FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
CMPIObjectPath* _makePath_NextHopIPRoute(const CMPIBroker *_broker,
                                         const CMPIContext *ctx,
                                         const CMPIObjectPath *cop,
                                         const struct nextHopIP *pNHop,
                                         CMPIStatus *rc)
{
    _OSBASE_TRACE(2, ("--- _makePath_NextHopIPRoute() called"));

    CMPIObjectPath *op = NULL; // ObjectPath to return.

    // The sblim-cmpi-base package offers some tool methods to get common
    // system datas
    // CIM_HOST_NAME contains the unique hostname of the local system
    if(!get_system_name()) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "no host name found");
        _OSBASE_TRACE(2, ("--- _makePath_NextHopIPRoute() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    op = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(cop,rc)),
                         _ClassName, rc);
    if(CMIsNullObject(op)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Create CMPIObjectPath failed.");
        _OSBASE_TRACE(2, ("--- _makePath_NextHopIPRoute() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    CMAddKey(op, "InstanceID", pNHop->instanceID, CMPI_chars);

 out:
    _OSBASE_TRACE(2, ("--- _makePath_NextHopIPRoute() exited"));
    return op;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIInstance* _makeInst_NextHopIPRoute(const CMPIBroker* _broker,
                                       const CMPIContext* ctx,
                                       const CMPIObjectPath* cop,
                                       const char** properties,
                                       const struct nextHopIP* pNHop,
                                       CMPIStatus* rc)
{
    _OSBASE_TRACE(2, ("--- _makeInst_NextHopIPRoute() called"));

    CMPIObjectPath  *op       = NULL; // Instance op.
    CMPIInstance    *ci       = NULL; // Instance to return.
    const char     **keys     = NULL; // Keys of the instances.
    int              keyCount = 0;    // Temporary counter.
    unsigned short   status   = 2;    // Enabled.

    // The sblim-cmpi-base package offers some tool methods to get common
    // system datas
    // CIM_HOST_NAME contains the unique hostname of the local system
    if(!get_system_name()) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "no host name found");
        _OSBASE_TRACE(2, ("--- _makeInst_NextHopIPRoute() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    op = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(cop,rc)),
                         _ClassName, rc);
    if(CMIsNullObject(op)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Create CMPIObjectPath failed.");
        _OSBASE_TRACE(2, ("--- _makeInst_NextHopIPRoute() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    ci = CMNewInstance(_broker, op, rc);
    if(CMIsNullObject(ci)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Create CMPIInstance failed.");
        _OSBASE_TRACE(2, ("--- _makeInst_NextHopIPRoute() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Set property filter.
    keys = calloc(2,sizeof(char*));
    keys[0] = strdup("InstanceID");
    keys[1] = NULL;
    CMSetPropertyFilter(ci, properties, keys);
    for( ; keys[keyCount]!=NULL ; keyCount++) {
        free((char*)keys[keyCount]);
    }
    FREE_SAFE(keys);

    CMSetProperty(ci, "AddressType", (CMPIValue*)&(pNHop->addressType),
                  CMPI_uint16);
    CMSetProperty(ci, "AdminDistance", (CMPIValue*)&(pNHop->adminDistance),
                  CMPI_uint16);
    CMSetProperty(ci, "Caption", pNHop->caption, CMPI_chars);
    CMSetProperty(ci, "Description", pNHop->description, CMPI_chars);
    CMSetProperty(ci, "DestinationAddress", pNHop->dstAddress, CMPI_chars);
    CMSetProperty(ci, "DestinationMask", pNHop->dstMask, CMPI_chars);
    CMSetProperty(ci, "ElementName", pNHop->elementName, CMPI_chars);
    CMSetProperty(ci, "InstanceID", pNHop->instanceID, CMPI_chars);
    CMSetProperty(ci, "IsStatic", (CMPIValue*)&(pNHop->isStatic), CMPI_boolean);
    CMSetProperty(ci, "OtherDerivation", pNHop->otherDerivation, CMPI_chars);
    CMSetProperty(ci, "PrefixLength", (CMPIValue*)&(pNHop->prefixLength),
                  CMPI_uint8);
    CMSetProperty(ci, "RouteDerivation", (CMPIValue*)&(pNHop->routeDerivation),
                  CMPI_uint16);
    CMSetProperty(ci, "RouteGateway", pNHop->routeGateway, CMPI_chars);
    CMSetProperty(ci, "RouteMetric", (CMPIValue*)&(pNHop->routeMetric),
                  CMPI_uint16);
    CMSetProperty(ci, "RouteOutputIf", (CMPIValue*)&(pNHop->routeOutputIf),
                  CMPI_uint16);
    CMSetProperty(ci, "RouteScope", (CMPIValue*)&(pNHop->routeScope),
                  CMPI_uint16);
    CMSetProperty(ci, "RouteTable", (CMPIValue*)&(pNHop->routeTable),
                  CMPI_uint16);
    CMSetProperty(ci, "RouteType", (CMPIValue*)&(pNHop->routeType),
                  CMPI_uint16);
    CMSetProperty(ci, "TypeOfRoute", (CMPIValue*)&(pNHop->typeOfRoute),
                  CMPI_uint16);

 out:
    _OSBASE_TRACE(2, ("--- _makeInst_NextHopIPRoute() exited"));
    return ci;
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
