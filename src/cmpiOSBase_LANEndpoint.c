/**
 * @file
 * cmpiOSBase_LANEndpoint.c
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
 * class Linux_LANEndpoint.
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
#include "cmpiOSBase_LANEndpoint.h"
#include "OSBase_LANEndpoint.h"


////////////////////////////////////////////////////////////////////////////////
// FACTORY FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
CMPIObjectPath* _makePath_LANEndpoint(const CMPIBroker *_broker,
                                      const CMPIContext *ctx,
                                      const CMPIObjectPath *cop,
                                      const struct LANEndpoint *LANEP,
                                      CMPIStatus *rc)
{
    _OSBASE_TRACE(2, ("--- _makePath_LANEndpoint() called"));

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
        _OSBASE_TRACE(2, ("--- _makePath_LANEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    CMAddKey(op, "Name", LANEP->name, CMPI_chars);
    CMAddKey(op, "CreationClassName", LANEP->creationClassName, CMPI_chars);
    CMAddKey(op, "SystemCreationClassName", CSCreationClassName,
             CMPI_chars);
    CMAddKey(op, "SystemName", LANEP->systemName, CMPI_chars);

 out:
    _OSBASE_TRACE(2, ("--- _makePath_LANEndpoint() exited"));
    return op;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIInstance* _makeInst_LANEndpoint(const CMPIBroker *_broker,
                                    const CMPIContext *ctx,
                                    const CMPIObjectPath *cop,
                                    const char **properties,
                                    const struct LANEndpoint *LANEP,
                                    CMPIStatus *rc)
{
    _OSBASE_TRACE(2, ("--- _makeInst_LANEndpoint() called"));

    CMPIObjectPath  *op         = NULL; // Instance op.
    CMPIInstance    *ci         = NULL; // Instance to return.
    const char     **keys       = NULL; // Keys of the instances.
    int              keyCount   = 0;    // Temporary counter.
    unsigned short   status     = 2;    // Enabled.
    CMPIArray       *opstatus   = NULL; // OperationalStatus.
    CMPIArray       *statusDesc = NULL; // StatusDescriptions.
    CMPIArray       *groupAddr  = NULL; // GroupAddresses.
    CMPIArray       *aliasAddr  = NULL; // AliasAddresses.
    CMPIArray       *avalRS     = NULL; // AvailableRequestedStates.

    // The sblim-cmpi-base package offers some tool methods to get common
    // system datas
    // CIM_HOST_NAME contains the unique hostname of the local system
    if(!get_system_name()) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "no host name found");
        _OSBASE_TRACE(2, ("--- _makeInst_LANEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    op = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(cop,rc)),
                         _ClassName, rc);
    if(CMIsNullObject(op)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Create CMPIObjectPath failed.");
        _OSBASE_TRACE(2, ("--- _makeInst_LANEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    ci = CMNewInstance(_broker, op, rc);
    if(CMIsNullObject(ci)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Create CMPIInstance failed.");
        _OSBASE_TRACE(2, ("--- _makeInst_LANEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Set property filter.
    keys = calloc(5, sizeof(char*));
    keys[0] = strdup("Name");
    keys[1] = strdup("CreationClassName");
    keys[2] = strdup("SystemCreationClassName");
    keys[3] = strdup("SystemName");
    keys[4] = NULL;
    CMSetPropertyFilter(ci, properties, keys);
    for( ; keys[keyCount]!=NULL ; keyCount++) {
        free((char*)keys[keyCount]);
    }
    FREE_SAFE(keys);

    aliasAddr = CMNewArray(_broker, 1, CMPI_chars, rc);
    if(aliasAddr == NULL) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not create AliasAddresses CMPIArray." );
        _OSBASE_TRACE(2, ("--- _makeInst_LANEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    } else {
        CMSetArrayElementAt(aliasAddr, 0, LANEP->aliasAddresses,
                            CMPI_chars);
        CMSetProperty(ci, "AliasAddresses",
                      (CMPIValue*)&(aliasAddr), CMPI_charsA);
    }

    avalRS = CMNewArray(_broker, ARRAY_SIZE(LANEP->availReqStates), CMPI_uint16, rc);
    if(avalRS == NULL) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not create AvailableRequestedStates CMPIArray." );
        _OSBASE_TRACE(2, ("--- _makeInst_LANEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    } else {
        int i;
        for(i=0 ; i<ARRAY_SIZE(LANEP->availReqStates) ; i++) {
            CMSetArrayElementAt(avalRS, i,
                                (CMPIValue*)&(LANEP->availReqStates[i]),
                                CMPI_uint16);
        }
        CMSetProperty(ci, "AvailableRequestedStates",
                      (CMPIValue*)&(avalRS), CMPI_uint16A);
    }

    CMSetProperty(ci, "Caption", LANEP->caption, CMPI_chars);
    CMSetProperty(ci, "CommunicationStatus",
                  (CMPIValue*)&(LANEP->communicationStatus), CMPI_uint16);
    CMSetProperty(ci, "CreationClassName", LANEP->creationClassName,
                  CMPI_chars);
    CMSetProperty(ci, "Description", LANEP->description, CMPI_chars);
    CMSetProperty(ci, "DetailedStatus", (CMPIValue*)&(LANEP->detailedStatus),
                  CMPI_uint16);
    CMSetProperty(ci, "ElementName", LANEP->elementName, CMPI_chars);
    CMSetProperty(ci, "EnabledDefault", (CMPIValue*)&(LANEP->enabledDefault),
                  CMPI_uint16);
    CMSetProperty(ci, "EnabledState", (CMPIValue*)&(LANEP->enabledState),
                  CMPI_uint16);

    groupAddr = CMNewArray(_broker, 1, CMPI_chars, rc);
    if(groupAddr == NULL) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not create GroupAddresses CMPIArray." );
        _OSBASE_TRACE(2, ("--- _makeInst_LANEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    } else {
        CMSetArrayElementAt(groupAddr, 0, LANEP->groupAddresses,
                            CMPI_chars);
        CMSetProperty(ci, "GroupAddresses",
                      (CMPIValue*)&(groupAddr), CMPI_charsA);
    }

    CMSetProperty(ci, "HealthState", (CMPIValue*)&(LANEP->healthState),
                  CMPI_uint16);
    // @todo how can obtain installation date form a device?
    // CMSetProperty(ci, "InstallDate", LANEP->, );
    CMSetProperty(ci, "InstanceID", LANEP->instanceID, CMPI_chars);
    CMSetProperty(ci, "LANID", LANEP->lanID, CMPI_chars);
    CMSetProperty(ci, "MACAddress", LANEP->macAddress, CMPI_chars);
    CMSetProperty(ci, "MaxDataSize", (CMPIValue*)&(LANEP->maxDataSize),
                  CMPI_uint32);
    CMSetProperty(ci, "Name", LANEP->name, CMPI_chars);
    CMSetProperty(ci, "NameFormat", LANEP->nameFormat, CMPI_chars);
    CMSetProperty(ci, "OperatingStatus",
                  (CMPIValue*)&(LANEP->operatingStatus), CMPI_uint16);

    opstatus = CMNewArray(_broker, 1, CMPI_uint16, rc);
    if(opstatus == NULL) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not create OperationalStatus CMPIArray." );
        _OSBASE_TRACE(2, ("--- _makeInst_LANEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    } else {
        CMSetArrayElementAt(opstatus, 0,
                            (CMPIValue*)&(LANEP->operationalStatus),
                            CMPI_uint16);
        CMSetProperty(ci, "OperationalStatus",
                      (CMPIValue*)&(opstatus), CMPI_uint16A);
    }

    CMSetProperty(ci, "OtherEnabledState", LANEP->otherEnabledState,
                  CMPI_chars);
    CMSetProperty(ci, "OtherTypeDescription", LANEP->otherTypeDescription,
                  CMPI_chars);
    CMSetProperty(ci, "PrimaryStatus", (CMPIValue*)&(LANEP->primaryStatus),
                  CMPI_uint16);
    CMSetProperty(ci, "ProtocolIFType", (CMPIValue*)&(LANEP->protocolIFType),
                  CMPI_uint16);
    CMSetProperty(ci, "RequestedState", (CMPIValue*)&(LANEP->requestedState),
                  CMPI_uint16);

    statusDesc = CMNewArray(_broker, 1, CMPI_chars, rc);
    if(statusDesc == NULL) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not create StatusDescriptions CMPIArray." );
        _OSBASE_TRACE(2, ("--- _makeInst_LANEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    } else {
        CMSetArrayElementAt(statusDesc, 0, LANEP->statusDescriptions,
                            CMPI_chars);
        CMSetProperty(ci, "StatusDescriptions",
                      (CMPIValue*)&(statusDesc), CMPI_charsA);
    }

    CMSetProperty(ci, "SystemCreationClassName", CSCreationClassName,
                  CMPI_chars);
    CMSetProperty(ci, "SystemName", LANEP->systemName, CMPI_chars);
    // @todo How can obtain device last change?.
    // CMSetProperty(ci, "TimeOfLastStateChange", LANEP->, );
    CMSetProperty(ci, "TransitioningToState",
                  (CMPIValue*)&(LANEP->transitioningToState), CMPI_uint16);

 out:
    _OSBASE_TRACE(2, ("--- _makeInst_LANEndpoint() exited"));
    return ci;
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
