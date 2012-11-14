/**
 * @file
 * cmpiOSBase_LANEndpointProvider.c
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
 * @description Linux_LANEndpoint class provider implementation.
 *              The following CMPI instance methods are supported:
 *               - Cleanup
 *               - EnumerateInstanceNames
 *               - EnumerateInstances
 *               - GetInstance
 *               - MethodCleanup
 *               - InvokeMethod
 *
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"
#include "OSBase_Common.h"
#include "cmpiOSBase_Common.h"
#include "cmpiOSBase_LANEndpoint.h"


//------------------------------------------------------------------------------
static const CMPIBroker *_broker;

#ifdef CMPI_VER_100
#define OSBase_LANEndpointProviderSetInstance OSBase_LANEndpointProviderModifyInstance
#endif //CMPI_VER_100
//------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
// INSTANCE PROVIDER INTERFACE
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
CMPIStatus OSBase_LANEndpointProviderCleanup(CMPIInstanceMI *mi,
                                             const CMPIContext *ctx,
                                             CMPIBoolean terminating)
{
    _OSBASE_TRACE(1, ("--- %s CMPI Cleanup() called", _ClassName));

    CMReturn(CMPI_RC_OK);

    _OSBASE_TRACE(1, ("--- %s CMPI Cleanup() exited", _ClassName));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_LANEndpointProviderEnumInstanceNames(CMPIInstanceMI *mi,
                                                       const CMPIContext *ctx,
                                                       const CMPIResult *rslt,
                                                       const CMPIObjectPath *ref)
{
    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() called", _ClassName));

    CMPIObjectPath         *op    = NULL;               // Object path instance.
    struct LANEndpointList *list  = NULL;               // LANEndpoint list.
    struct LANEndpointList *lptr  = NULL;               // Current position in
                                                        // the list.
    CMPIStatus              rc    = {CMPI_RC_OK, NULL}; // Return status of
                                                        // CIM operations.

    // Get links.
    if(getALLLANEndpoints(&list) != LEP_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not list LANEndpoints.");
        _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Put pointer to the begining of the list.
    lptr = list;

    // Iterate routes list.
    while(lptr != NULL && rc.rc == CMPI_RC_OK) {
        if(lptr->sptr != NULL) {
            op = _makePath_LANEndpoint(_broker, ctx, ref, lptr->sptr, &rc);
            if((op == NULL) || (rc.rc != CMPI_RC_OK)) {
                if(rc.msg != NULL) {
                    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() " \
                                      "failed: %s", _ClassName,
                                      CMGetCharPtr(rc.msg)));
                }
                CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                     "Transformation from internal structure " \
                                     "to CIM Instance failed.");
                _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() failed: %s",
                                  _ClassName, CMGetCharPtr(rc.msg)));
                goto out;
            } else {
                CMReturnObjectPath(rslt, op);
            }
        }
        lptr = lptr->next;
    }

 out:
    freeLANEndpointList(lptr);
    freeLANEndpointList(list);
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_LANEndpointProviderEnumInstances(CMPIInstanceMI *mi,
                                                   const CMPIContext *ctx,
                                                   const CMPIResult *rslt,
                                                   const CMPIObjectPath *ref,
                                                   const char **properties)
{
    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() called", _ClassName));

    CMPIInstance           *ci    = NULL;               // Class instance.
    struct LANEndpointList *list  = NULL;               // LANEndpoint list.
    struct LANEndpointList *lptr  = NULL;               // Current position in
                                                        // the list.
    CMPIStatus              rc    = {CMPI_RC_OK, NULL}; // Return status of
                                                        // CIM operations.

    // Get links.
    if(getALLLANEndpoints(&list) != LEP_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not list LANEndpoints.");
        _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Put pointer to the begining of the list.
    lptr = list;

    // Iterate routes list.
    while(lptr != NULL && rc.rc == CMPI_RC_OK) {
        if(lptr->sptr != NULL) {
            ci = _makeInst_LANEndpoint(_broker, ctx, ref, properties,
                                       lptr->sptr, &rc);
            if((ci == NULL) || (rc.rc != CMPI_RC_OK)) {
                if(rc.msg != NULL) {
                    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() failed: %s",
                                      _ClassName, CMGetCharPtr(rc.msg)));
                }
                CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                     "Transformation from internal structure " \
                                     "to CIM Instance failed.");
                _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() failed: %s",
                                  _ClassName, CMGetCharPtr(rc.msg)));
                goto out;
            } else {
                CMReturnInstance(rslt, ci);
            }
        }
        lptr = lptr->next;
    }

 out:
    freeLANEndpointList(lptr);
    freeLANEndpointList(list);
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_LANEndpointProviderGetInstance(CMPIInstanceMI *mi,
                                                 const CMPIContext *ctx,
                                                 const CMPIResult *rslt,
                                                 const CMPIObjectPath *cop,
                                                 const char **properties)
{
    _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() called", _ClassName));

    CMPIInstance           *ci    = NULL;               // Class instance.
    CMPIData                data;                       // CIM datum to extract
                                                        // property values.
    struct LANEndpointList *list  = NULL;               // LANEndpoint list.
    CMPIStatus              rc    = {CMPI_RC_OK, NULL}; // Return status of
                                                        // CIM operations.

    // Get Name.
    data = CMGetKey(cop, "Name", &rc);
    if((rc.rc != CMPI_RC_OK) || (CMIsNullValue(data)) ||
        (data.type != CMPI_string)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                             "Could not get LANEndpoint Name.");
        _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Get link info.
    if((getLANEndpoint(&list, CMGetCharPtr(data.value.string)) != LEP_OK) ||
       (list == NULL)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_FOUND,
                             "LANEndpoint does not exist.");
        _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Create new instance.
    ci = _makeInst_LANEndpoint(_broker, ctx, cop, properties,
                               list->sptr, &rc);
    if(ci == NULL) {
        if(rc.msg != NULL) {
            _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
        } else {
            _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() failed",
                              _ClassName));
        }
        goto out;
    }
    CMReturnInstance(rslt, ci);

 out:
    freeLANEndpointList(list);
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_LANEndpointProviderCreateInstance(CMPIInstanceMI *mi,
                                                    const CMPIContext *ctx,
                                                    const CMPIResult *rslt,
                                                    const CMPIObjectPath *cop,
                                                    const CMPIInstance *ci)
{
    _OSBASE_TRACE(1, ("--- %s CMPI CreateInstance() called", _ClassName));

    CMPIStatus rc = {CMPI_RC_OK, NULL}; // Return status of CIM operations.

    CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_SUPPORTED,
                         "CIM_ERR_NOT_SUPPORTED");
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI CreateInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_LANEndpointProviderSetInstance(CMPIInstanceMI *mi,
                                                 const CMPIContext *ctx,
                                                 const CMPIResult *rslt,
                                                 const CMPIObjectPath *cop,
                                                 const CMPIInstance *ci,
                                                 const char **properties)
{
    _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() called", _ClassName));

    CMPIStatus rc = {CMPI_RC_OK, NULL}; // Return status of CIM operations.

    CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_SUPPORTED,
                         "CIM_ERR_NOT_SUPPORTED");
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_LANEndpointProviderDeleteInstance(CMPIInstanceMI *mi,
                                                    const CMPIContext *ctx,
                                                    const CMPIResult *rslt,
                                                    const CMPIObjectPath *cop)
{
    _OSBASE_TRACE(1, ("--- %s CMPI DeleteInstance() called", _ClassName));

    CMPIStatus rc = {CMPI_RC_OK, NULL}; // Return status of CIM operations.

    CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_SUPPORTED,
                         "CIM_ERR_NOT_SUPPORTED");
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI DeleteInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_LANEndpointProviderExecQuery(CMPIInstanceMI *mi,
                                               const CMPIContext *ctx,
                                               const CMPIResult *rslt,
                                               const CMPIObjectPath *ref,
                                               const char *lang,
                                               const char *query)
{
    _OSBASE_TRACE(1, ("--- %s CMPI ExecQuery() called", _ClassName));

    CMPIStatus rc = {CMPI_RC_OK, NULL}; // Return status of CIM operations.

    CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_SUPPORTED,
                         "CIM_ERR_NOT_SUPPORTED");
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI ExecQuery() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// METHOD PROVIDER INTERFACE
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
CMPIStatus OSBase_LANEndpointProviderMethodCleanup(CMPIMethodMI *mi,
                                                   const CMPIContext *ctx,
                                                   CMPIBoolean terminating)
{
    _OSBASE_TRACE(1, ("--- %s CMPI MethodCleanup() called", _ClassName));

    CMReturn(CMPI_RC_OK);

    _OSBASE_TRACE(1, ("--- %s CMPI MethodCleanup() exited", _ClassName));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_LANEndpointProviderInvokeMethod(CMPIMethodMI *mi,
                                                  const CMPIContext *ctx,
                                                  const CMPIResult *rslt,
                                                  const CMPIObjectPath *ref,
                                                  const char *methodName,
                                                  const CMPIArgs *in,
                                                  CMPIArgs *out)
{
    _OSBASE_TRACE(1, ("--- %s CMPI InvokeMethod() called", _ClassName));

    CMPIString *class = NULL;               // Class name.
    CMPIStatus  rc    = {CMPI_RC_OK, NULL}; // Return status of CIM operations.

    // Obtain class name.
    class = CMGetClassName(ref, &rc);

    // RequestStateChange
    if((strcasecmp(CMGetCharPtr(class), _ClassName) == 0) &&
       (strcasecmp("RequestStateChange", methodName) == 0)) {
        CMPIInstance *instance     = NULL;       // Target device instance.
        CMPIData      data;                      // CIM datum to extract
                                                 // property values.
        CMPIString   *name         = NULL;       // LANEndpoint Name.
        CMPIUint16    enabledState;              // Indicates the enabled and
                                                 // disabled states of the
                                                 // device.
        CMPIString   *otherEnSt    = NULL;       // A string that describes the
                                                 // enabled or disabled state
                                                 // of the element when the
                                                 // EnabledState property is
                                                 // set to 1.
        CMPIUint16    reqState;                  // Desired state for the
                                                 // device.
        CMPIString   *timeoutP     = NULL;       // A timeout period that
                                                 // specifies the maximum
                                                 // amount of time that the
                                                 // client expects the
                                                 // transition to the new state
                                                 // to take. (must be a
                                                 // datetime interval ->
                                                 // ddddddddhhmmss.mmmmmm:000).
        CMPIBoolean   skipTimeoutP = CMPI_false; // If TimeoutPeriod is NULL or
                                                 // 0, indicates that the
                                                 // client has no time
                                                 // requirements for the
                                                 // transition.
        CMPIDateTime *timeLtStCh   = NULL;       // The date or time when the
                                                 // EnabledState of the element
                                                 // last changed.
        CMPIUint16    result;                    // Result of state change.
        char          resError[256];             // Result error description.
        
        // Get the LANEndpoint name from the reference object path.
        data = CMGetKey(ref, "Name", &rc);
        if((rc.rc == CMPI_RC_OK) && !CMIsNullValue(data)) {
            name = data.value.string;
        }
        if(name == NULL) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_FOUND,
                                 "Cannot determine Name");
            _OSBASE_TRACE(1, ("%s CMPI InvokeMethod() [%s] failed: %s",
                              _ClassName, methodName, CMGetCharPtr(rc.msg)));
            goto out;
        }

        // Get the requested state from the input args.
        data = CMGetArg(in, "RequestedState", &rc);
        if((rc.rc == CMPI_RC_OK) && !CMIsNullValue(data)) {
            if(data.type == CMPI_uint16) {
                reqState = data.value.uint16;
            } else if(data.type == CMPI_string) {
                reqState = (CMPIUint16)atoi(CMGetCharPtr(data.value.string));
            }
        }

        // Get the timeout period from the input args.
        data = CMGetArg(in, "TimeoutPeriod", &rc);
        if(rc.rc == CMPI_RC_OK) {
            if(!CMIsNullValue(data)) {
                if(data.type == CMPI_dateTime) {
                    // TimeoutPeriod must be datetime interval.
                    if(!CMIsInterval(data.value.dateTime, &rc)) {
                        CMSetStatusWithChars(_broker, &rc,
                                             CMPI_RC_ERR_INVALID_PARAMETER,
                                             "TimeoutPeriod must be datetime " \
                                             "interval. "               \
                                             "(ddddddddhhmmss.mmmmmm:000)");
                        _OSBASE_TRACE(1, ("%s CMPI InvokeMethod() [%s] " \
                                          "failed: %s", _ClassName, methodName,
                                          CMGetCharPtr(rc.msg)));
                        goto out;
                    } else {
                        timeoutP = CMGetStringFormat(data.value.dateTime, &rc);
                    }
                } else if(data.type == CMPI_string) {
                    timeoutP = data.value.string;
                }
            } else {
                skipTimeoutP = CMPI_true;
            }
        }
        if(!skipTimeoutP && timeoutP == NULL) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                                 "Cannot determine TimeoutPeriod");
            _OSBASE_TRACE(1, ("%s CMPI InvokeMethod() [%s] failed: %s",
                              _ClassName, methodName, CMGetCharPtr(rc.msg)));
            goto out;
        }

        // Getting instance from the specified Device.
        instance = CBGetInstance(_broker, ctx, ref, NULL, &rc);
        if((rc.rc != CMPI_RC_OK) && CMIsNullObject(instance)) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                 "Cannot get the instance from the specified"
                                 "Device");
            _OSBASE_TRACE(1, ("%s CMPI InvokeMethod() [%s] failed: %s",
                              _ClassName, methodName, CMGetCharPtr(rc.msg)));
            goto out;
        }

        // Setting RequestedState.
        CMSetProperty(instance, "RequestedState",
                      (CMPIValue*)&(reqState), CMPI_uint16);

        // Setting EnabledState.
        enabledState = reqState;

        // Change link Operational status.
        if(!skipTimeoutP) {
            CMPIUint64 convertedDateTime =
                datetime_str_interval_to_ms(CMGetCharPtr(timeoutP));
            result = changeLinkOPState(CMGetCharPtr(name),
                                        enabledState, convertedDateTime);
        } else {
            result = changeLinkOPState(CMGetCharPtr(name),
                                        enabledState, 0);
        }
        if(result != 0) {
            snprintf(resError, 256, "Result code %d", result);
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERROR, resError);
            _OSBASE_TRACE(1, ("%s CMPI InvokeMethod() [%s] failed: %s",
                              _ClassName, methodName, CMGetCharPtr(rc.msg)));
            goto out;
        }
        CMSetProperty(instance, "EnabledState",
                      (CMPIValue*)&(enabledState), CMPI_uint16);

        // Setting TimeOfLastStateChange.
        timeLtStCh = CMNewDateTime(_broker, &rc);
        CMSetProperty(instance, "TimeOfLastStateChange",
                      (CMPIValue*)&(timeLtStCh), CMPI_dateTime);
        
    out:
        _OSBASE_TRACE(1, ("%s CMPI InvokeMethod() [%s] failed: %s",
                          _ClassName, methodName,
                          (rc.rc == CMPI_RC_OK)?"succeeded":"failed"));
    } else {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_FOUND,
                             methodName);
    }
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI InvokeMethod() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROVIDER FACTORY
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
CMInstanceMIStub(OSBase_LANEndpointProvider,
                 OSBase_LANEndpointProvider,
                 _broker,
                 CMNoHook);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMMethodMIStub(OSBase_LANEndpointProvider,
               OSBase_LANEndpointProvider,
               _broker,
               CMNoHook);
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
