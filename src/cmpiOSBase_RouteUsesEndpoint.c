/**
 * @file
 * cmpiOSBase_RouteUsesEndpoint.c
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
 * class Linux_RouteUsesEndpoint.
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
#include "OSBase_Netlink.h"
#include "cmpiOSBase_RouteUsesEndpoint.h"


////////////////////////////////////////////////////////////////////////////////
// FACTORY FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
CMPIObjectPath* _makePath_RouteUsesEndpoint(const CMPIBroker *_broker,
                                            const CMPIObjectPath *ops,
                                            const CMPIObjectPath *opt,
                                            CMPIStatus *rc)
{
    _OSBASE_TRACE(4, ("--- _makePath_RouteUsesEndpoint() called"));

    CMPIObjectPath *op = NULL; // Assoc objectPath.
    CMPIInstance   *ci = NULL; // Assoc instance.

    // Get assoc instance.
    ci = _makeInst_RouteUsesEndpoint(_broker, ops, opt, rc);
    if(CMIsNullObject(ci)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not create instance.");
        _OSBASE_TRACE(2, ("--- _makePath_RouteUsesEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Get assoc objectPath.
    op = CMGetObjectPath(ci, rc);
    if(CMIsNullObject(op)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not get objectPath.");
        _OSBASE_TRACE(2, ("--- _makePath_RouteUsesEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }
    CMSetNameSpace(op, CMGetCharPtr(CMGetNameSpace(ops, rc)));

 out:
    _OSBASE_TRACE(4, ("--- _makePath_RouteUsesEndpoint() exited"));
    return op;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIInstance* _makeInst_RouteUsesEndpoint(const CMPIBroker *_broker,
                                          const CMPIObjectPath *ops,
                                          const CMPIObjectPath *opt,
                                          CMPIStatus *rc)
{
    _OSBASE_TRACE(4, ("--- _makeInst_RouteUsesEndpoint() called"));

    CMPIObjectPath *op = NULL; // Assoc objectPath.
    CMPIInstance   *ci = NULL; // Assoc instance.

    // Create assoc objectPath.
    op = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(ops, rc)),
                         _ClassName, rc);
    if(CMIsNullObject(op)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not get objectPath.");
        _OSBASE_TRACE(2, ("--- _makeInst_RouteUsesEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Create assoc instance.
    ci = CMNewInstance(_broker, op, rc);
    if(CMIsNullObject(ci)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not create instance.");
        _OSBASE_TRACE(2, ("--- _makeInst_RouteUsesEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    CMSetProperty(ci, _RefLeft, (CMPIValue*)&(ops), CMPI_ref);
    CMSetProperty(ci, _RefRight, (CMPIValue*)&(opt), CMPI_ref);

 out:
    _OSBASE_TRACE(4, ("--- _makeInst_RouteUsesEndpoint() exited"));
    return ci;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus* _assoc_get_NextHopRoute_insts(const CMPIBroker *_broker,
                                          const CMPIContext *ctx,
                                          const CMPIResult *rslt,
                                          const CMPIObjectPath *ref,
                                          const CMPIInstance *sourceInst,
                                          const char *targetClass,
                                          const int assocType,
                                          CMPIStatus *rc)
{
    _OSBASE_TRACE(2, ("--- _assoc_get_NextHopRoute_insts() called"));

    CMPIEnumeration *cet          = NULL; // Target class enumeration.
    CMPIObjectPath  *opt          = NULL; // Target objectPath.
    CMPIObjectPath  *opit         = NULL; // Target instance objectPath.
    CMPIData         instData;            // Target instance data.
    CMPIData         data;                // CIM datum to extract property
                                          // values.
    CMPIInstance    *rcit         = NULL; // Result reference instance.
    CMPIObjectPath  *ropt         = NULL; // Result reference objectPath.
    int              ifindex;             // Interface index.
    char             resultClass[256];    // @todo remove.

    // @todo remove this, we should found a better way to handle this.
    if(strcasecmp(targetClass, _ClassName) == 0) {
        snprintf(resultClass, 256, "%s", _RefRightClasses[0]);
    } else {
        snprintf(resultClass, 256, "%s", targetClass);
    }

    // Get ElementName.
    data = CMGetProperty(sourceInst, "ElementName", rc);
    if((rc->rc != CMPI_RC_OK) || (CMIsNullValue(data)) ||
       (data.type != CMPI_string)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                             "Cannot determine ElementName");
        _OSBASE_TRACE(2, ("--- _assoc_get_NextHopRoute_insts() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Get interface index.
    sscanf(CMGetCharPtr(data.value.string), "%*[^_]_%d", &ifindex);

    // Create ObjectPath of result class.
    opt = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(ref, rc)),
                          resultClass, rc);
    if(CMIsNullObject(opt)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Create CMPIObjectPath failed.");
        _OSBASE_TRACE(2, ("--- _assoc_get_NextHopRoute_insts() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Enumerate result instances.
    cet = CBEnumInstances(_broker, ctx, opt, NULL, rc);
    if(CMIsNullObject(cet)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not enumerate result instances.");
        _OSBASE_TRACE(2, ("--- _assoc_get_NextHopRoute_insts() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Retrieve instances.
    while(CMHasNext(cet, rc)) {
        instData = CMGetNext(cet, rc);
        if(CMIsNullObject(instData.value.inst)) {
            CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                                 "Could not retrieve instance from "    \
                                 "enumeration.");
            _OSBASE_TRACE(2, ("--- _assoc_get_NextHopRoute_insts() failed: %s",
                              CMGetCharPtr(rc->msg)));
            goto out;
        }

        // Get RouteOutputIf.
        data = CMGetProperty(instData.value.inst, "RouteOutputIf", rc);
        if((rc->rc != CMPI_RC_OK) || (CMIsNullValue(data)) ||
           (data.type != CMPI_uint16)) {
            CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                                 "Cannot determine RouteOutputIf");
            _OSBASE_TRACE(2, ("--- _assoc_get_NextHopRoute_insts() failed: %s",
                              CMGetCharPtr(rc->msg)));
            goto out;
        }

        if((int)(data.value.uint16) == ifindex) {
            // Get objectPath.
            opit = CMGetObjectPath(instData.value.inst, rc);
            if(CMIsNullObject(opit)) {
                CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                                     "Could not get objectPath.");
                _OSBASE_TRACE(2, ("--- _assoc_get_NextHopRoute_insts() " \
                                  "failed: %s", CMGetCharPtr(rc->msg)));
                goto out;
            }
            CMSetNameSpace(opit, CMGetCharPtr(CMGetNameSpace(ref, rc)));

            if(assocType == ATYPE_ASSOC) { // Associators.
                CMReturnInstance(rslt, instData.value.inst);
            } else if(assocType == ATYPE_ASSOCN) { // AssociatorNames.
                CMReturnObjectPath(rslt, opit);
            } else if(assocType == ATYPE_REFER) { // References.
                rcit = _makeInst_RouteUsesEndpoint(_broker, ref, opit, rc);
                CMReturnInstance(rslt, rcit);
            } else if(assocType == ATYPE_REFERN) { // ReferenceNames.
                ropt = _makePath_RouteUsesEndpoint(_broker, ref, opit, rc);
                CMReturnObjectPath(rslt, ropt);
            } else {
                CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_NOT_SUPPORTED,
                                     "Wrong association type or not "   \
                                     "supported.");
                _OSBASE_TRACE(2, ("--- __assoc_get_NextHopRoute_insts() failed: %s",
                                  CMGetCharPtr(rc->msg)));
                goto out;
            }
        }
    }

 out:
    _OSBASE_TRACE(2, ("--- _assoc_get_NextHopRoute_insts() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus* _assoc_get_ProtocolEndpoint_insts(const CMPIBroker *_broker,
                                              const CMPIContext *ctx,
                                              const CMPIResult *rslt,
                                              const CMPIObjectPath *ref,
                                              const CMPIInstance *sourceInst,
                                              const char *targetClass,
                                              const int assocType,
                                              CMPIStatus *rc)
{
    _OSBASE_TRACE(2, ("--- _assoc_get_ProtocolEndpoint_insts() called"));

    CMPIEnumeration *cet          = NULL; // Target class enumeration.
    CMPIObjectPath  *opt          = NULL; // Target objectPath.
    CMPIObjectPath  *opit         = NULL; // Target instance objectPath.
    CMPIData         instData;            // Target instance data.
    CMPIData         data;                // CIM datum to extract property
                                          // values.
    CMPIInstance    *rcit         = NULL; // Result reference instance.
    CMPIObjectPath  *ropt         = NULL; // Result reference objectPath.
    int              ifindex;             // Interface index.
    int              tmpIndex;            // Temp store ifindex of an instance.
    char             resultClass[256];    // @todo remove.

    // @todo remove this, we should found a better way to handle this.
    if(strcasecmp(targetClass, _ClassName) == 0) {
        snprintf(resultClass, 256, "%s", _RefLeftClasses[0]);
    } else {
        snprintf(resultClass, 256, "%s", targetClass);
    }

    // Get RouteOutputIf.
    data = CMGetProperty(sourceInst, "RouteOutputIf", rc);
    if((rc->rc != CMPI_RC_OK) || (CMIsNullValue(data)) ||
       (data.type != CMPI_uint16)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                             "Cannot determine RouteOutputIf");
        _OSBASE_TRACE(2, ("--- _assoc_get_ProtocolEndpoint_insts() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Get interface index.
    ifindex = (int)(data.value.uint16);

    // Create ObjectPath of result class.
    opt = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(ref,rc)),
                          resultClass, rc);
    if(CMIsNullObject(opt)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Create CMPIObjectPath failed.");
        _OSBASE_TRACE(2, ("--- _assoc_get_ProtocolEndpoint_insts() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Enumerate result instances.
    cet = CBEnumInstances(_broker, ctx, opt, NULL, rc);
    if(CMIsNullObject(cet)) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not enumerate result instances.");
        _OSBASE_TRACE(2, ("--- _assoc_get_ProtocolEndpoint_insts() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Retrieve instances.
    while(CMHasNext(cet, rc)) {
        instData = CMGetNext(cet, rc);
        if(CMIsNullObject(instData.value.inst)) {
            CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                                 "Could not retrieve instance from "    \
                                 "enumeration.");
            _OSBASE_TRACE(2, ("--- _assoc_get_ProtocolEndpoint_insts() " \
                              "failed: %s", CMGetCharPtr(rc->msg)));
            goto out;
        }

        // Get ElementName.
        data = CMGetProperty(instData.value.inst, "ElementName", rc);
        if((rc->rc != CMPI_RC_OK) || (CMIsNullValue(data)) ||
           (data.type != CMPI_string)) {
            CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                                 "Cannot determine ElementName");
            _OSBASE_TRACE(2, ("--- _assoc_get_ProtocolEndpoint_insts() " \
                              "failed: %s", CMGetCharPtr(rc->msg)));
            goto out;
        }

        // Get interface index.
        sscanf(CMGetCharPtr(data.value.string), "%*[^_]_%d", &tmpIndex);

        if(tmpIndex == ifindex) {
            // Get objectPath.
            opit = CMGetObjectPath(instData.value.inst, rc);
            if(CMIsNullObject(opit)) {
                CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                                     "Could not get objectPath.");
                _OSBASE_TRACE(2, ("--- _assoc_get_ProtocolEndpoint_insts() " \
                                  "failed: %s", CMGetCharPtr(rc->msg)));
                goto out;
            }
            CMSetNameSpace(opit, CMGetCharPtr(CMGetNameSpace(ref, rc)));

            if(assocType == ATYPE_ASSOC) { // Associators.
                CMReturnInstance(rslt, instData.value.inst);
            } else if(assocType ==  ATYPE_ASSOCN) { // AssociatorNames.
                CMReturnObjectPath(rslt, opit);
            } else if(assocType == ATYPE_REFER) { // References.
                rcit = _makeInst_RouteUsesEndpoint(_broker, ref, opit, rc);
                CMReturnInstance(rslt, rcit);
            } else if(assocType == ATYPE_REFERN) { // ReferenceNames.
                ropt = _makePath_RouteUsesEndpoint(_broker, ref, opit, rc);
                CMReturnObjectPath(rslt, ropt);
            } else {
                CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_NOT_SUPPORTED,
                                     "Wrong association type or not "   \
                                     "supported.");
                _OSBASE_TRACE(2, ("--- _assoc_get_ProtocolEndpoint_insts() "\
                                  "failed: %s", CMGetCharPtr(rc->msg)));
                goto out;
            }
        }
    }

 out:
    _OSBASE_TRACE(2, ("--- _assoc_get_ProtocolEndpoint_insts() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus _assoc_RouteUsesEndpoint(const CMPIBroker *_broker,
                                    const CMPIContext *ctx,
                                    const CMPIResult *rslt,
                                    const CMPIObjectPath *ref,
                                    const char *targetClass,
                                    const int assocType,
                                    CMPIStatus *rc)
{
    _OSBASE_TRACE(2, ("--- _assoc_RouteUsesEndpoint() called"));

    CMPIInstance   *cis          = NULL; // Source class instance.
    CMPIString     *srcClassName = NULL; // Source class name.
    int             i, instFound = 0;

    // Check if source instance does exist.
    cis = CBGetInstance(_broker, ctx, ref, NULL, rc);
    if(CMIsNullObject(cis)) {
        if(rc->rc == CMPI_RC_ERR_FAILED) {
            CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                                 "GetInstance of source object failed.");
        }
        if(rc->rc == CMPI_RC_ERR_NOT_FOUND) {
            CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_NOT_FOUND,
                                 "Source object not found.");
        }
        _OSBASE_TRACE(2, ("--- _assoc_RouteUsesEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Get source class name.
    srcClassName = CMGetClassName(ref, rc);
    if(CMGetCharPtr(srcClassName) == NULL) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_FAILED,
                             "Could not retrieve source class name.");
        _OSBASE_TRACE(2, ("--- _assoc_RouteUsesEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

    // Check if source class is one of the supported right classes.
    for(i=0 ; i<ARRAY_SIZE(_RefRightClasses) ; i++) {
        if(strcasecmp(CMGetCharPtr(srcClassName),
                      _RefRightClasses[i]) == 0) {
            // Returned instances are subclasses of type CIM_ProtocolEndpoint.
            rc = _assoc_get_ProtocolEndpoint_insts(_broker, ctx, rslt, ref, cis,
                                                   targetClass, assocType, rc);
            instFound = 1;
        }
    }

    // Check if source class is one of the supported left classes.
    if(!instFound) {
        for(i=0 ; i<ARRAY_SIZE(_RefLeftClasses) ; i++) {
            if(strcasecmp(CMGetCharPtr(srcClassName),
                          _RefLeftClasses[i]) == 0) {
                // Returned instances are subclasses of type CIM_NextHopRoute.
                rc = _assoc_get_NextHopRoute_insts(_broker, ctx, rslt, ref, cis,
                                                   targetClass, assocType, rc);
                instFound = 1;
            }
        }
    }

    // Target class not supported.
    if(!instFound) {
        CMSetStatusWithChars(_broker, rc, CMPI_RC_ERR_NOT_SUPPORTED,
                             "Association not supported "
                             "(check source and target classes).");
        _OSBASE_TRACE(2, ("--- _assoc_RouteUsesEndpoint() failed: %s",
                          CMGetCharPtr(rc->msg)));
        goto out;
    }

 out:
    _OSBASE_TRACE(2, ("--- _assoc_RouteUsesEndpoint() exited"));
    return (*rc);
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
