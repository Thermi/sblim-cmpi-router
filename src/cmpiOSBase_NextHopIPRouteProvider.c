/**
 * @file
 * cmpiOSBase_NextHopIPRouteProvider.c
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
 * @description Linux_NextHopIPRoute class provider implementation.
 *              The following CMPI instance methods are supported:
 *               - Cleanup
 *               - EnumerateInstanceNames
 *               - EnumerateInstances
 *               - GetInstance
 *               - CreateInstance
 *               - SetInstance
 *               - DeleteInstance
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
#include "cmpiOSBase_NextHopIPRoute.h"


//------------------------------------------------------------------------------
static const CMPIBroker *_broker;

#ifdef CMPI_VER_100
#define OSBase_NextHopIPRouteProviderSetInstance OSBase_NextHopIPRouteProviderModifyInstance
#endif //CMPI_VER_100
//------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
// HELPERS
////////////////////////////////////////////////////////////////////////////////

/**
 * Get parameters from CMPIInstance.
 * @param ci [in] CMPIInstance.
 * @param nHopIP [out] structure to be filled.
 * @param nHopIPPrev [in] previous route info.
 * @return 0=succesful | 1=fail
 */
CMPIStatus getNextHopIPParams(const CMPIInstance *ci, struct nextHopIP *nHopIP,
                              const struct nextHopIP *nHopIPPrev);

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// INSTANCE PROVIDER INTERFACE
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
CMPIStatus OSBase_NextHopIPRouteProviderCleanup(CMPIInstanceMI *mi,
                                                const CMPIContext *ctx,
                                                CMPIBoolean terminating)
{
    _OSBASE_TRACE(1, ("--- %s CMPI Cleanup() called", _ClassName));

    CMReturn(CMPI_RC_OK);

    _OSBASE_TRACE(1, ("--- %s CMPI Cleanup() exited", _ClassName));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_NextHopIPRouteProviderEnumInstanceNames(CMPIInstanceMI *mi,
                                                          const CMPIContext *ctx,
                                                          const CMPIResult *rslt,
                                                          const CMPIObjectPath *ref)
{
    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() called", _ClassName));

    CMPIObjectPath       *op    = NULL;               // Object path instance.
    struct nextHopIPList *list  = NULL;               // NextHopIP list.
    struct nextHopIPList *lptr  = NULL;               // Current position in the
                                                      // list.
    CMPIStatus            rc    = {CMPI_RC_OK, NULL}; // Return status of CIM
                                                      // operations.

    // Get routes.
    if(getAllIPRoutes(&list) != NH_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not list nextHot ip routes.");
        _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Put pointer to the begining of the list.
    lptr = list;

    // Iterate routes list.
    while(lptr != NULL && rc.rc == CMPI_RC_OK) {
        if(lptr->sptr != NULL) {
            op = _makePath_NextHopIPRoute(_broker, ctx, ref, lptr->sptr, &rc);
            if((op == NULL) || (rc.rc != CMPI_RC_OK)) {
                if(rc.msg != NULL) {
                    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() " \
                                     "failed: %s", _ClassName,
                                     CMGetCharPtr(rc.msg)));
                }
                CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                     "Transformation from internal structure "
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
    freeNextHopIPList(lptr);
    freeNextHopIPList(list);
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_NextHopIPRouteProviderEnumInstances(CMPIInstanceMI *mi,
                                                      const CMPIContext *ctx,
                                                      const CMPIResult *rslt,
                                                      const CMPIObjectPath *ref,
                                                      const char **properties)
{
    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() called", _ClassName));

    CMPIInstance         *ci    = NULL;               // Class instance.
    struct nextHopIPList *list  = NULL;               // NextHopIP list.
    struct nextHopIPList *lptr  = NULL;               // Current position
                                                      // in the list.
    CMPIStatus            rc    = {CMPI_RC_OK, NULL}; // Return status of CIM
                                                      // operations.

    // Get routes.
    if(getAllIPRoutes(&list) != NH_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not list nextHot ip routes.");
        _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Put pointer to the begining of the list.
    lptr = list;

    // Iterate routes list.
    while(lptr != NULL && rc.rc == CMPI_RC_OK) {
        if(lptr->sptr != NULL) {
            ci = _makeInst_NextHopIPRoute(_broker, ctx, ref, properties,
                                          lptr->sptr, &rc);
            if((ci == NULL) || (rc.rc != CMPI_RC_OK)) {
                if(rc.msg != NULL) {
                    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() failed: %s",
                                      _ClassName, CMGetCharPtr(rc.msg)));
                }
                CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                     "Transformation from internal structure "
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
    freeNextHopIPList(lptr);
    freeNextHopIPList(list);
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_NextHopIPRouteProviderGetInstance(CMPIInstanceMI *mi,
                                                    const CMPIContext *ctx,
                                                    const CMPIResult *rslt,
                                                    const CMPIObjectPath *cop,
                                                    const char **properties)
{
    _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() called", _ClassName));

    CMPIInstance         *ci    = NULL;               // Class instance.
    CMPIData              data;                       // CIM datum to extract
                                                      // property values.
    struct nextHopIPList *list  = NULL;               // NextHopIP list.
    CMPIStatus            rc    = {CMPI_RC_OK, NULL}; // Return status of
                                                      // CIM operations.

    // Get instanceId.
    data = CMGetKey(cop, "InstanceId", &rc);
    if((rc.rc != CMPI_RC_OK) || (CMIsNullValue(data)) ||
        (data.type != CMPI_string)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                             "Could not get NextHopIP InstanceID.");
        _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Get route info.
    if((getIPRouteIId(&list, CMGetCharPtr(data.value.string)) != NH_OK) ||
       (list == NULL)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_FOUND,
                             "NextHopIP does not exist.");
        _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Create new instance.
    ci = _makeInst_NextHopIPRoute(_broker, ctx, cop, properties,
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
    freeNextHopIPList(list);
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_NextHopIPRouteProviderCreateInstance(CMPIInstanceMI *mi,
                                                       const CMPIContext *ctx,
                                                       const CMPIResult *rslt,
                                                       const CMPIObjectPath *cop,
                                                       const CMPIInstance *ci)
{
    _OSBASE_TRACE(1, ("--- %s CMPI CreateInstance() called", _ClassName));

    struct nextHopIP *nHopIP = NULL;               // Route to be added.
    CMPIStatus        rc     = {CMPI_RC_OK, NULL}; // Return status of
                                                   // CIM operations.

    // Alloc struct.
    nHopIP = malloc(sizeof(*nHopIP));
    if(nHopIP == NULL) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_FOUND,
                             "Cannot alloc memory for \'nHopIP\'.");
        _OSBASE_TRACE(1, ("--- %s CMPI CreateInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }
    memset(nHopIP, 0, sizeof(*nHopIP));

    // Get nextHopIP info.
    rc = getNextHopIPParams(ci, nHopIP, NULL);
    if(rc.rc != CMPI_RC_OK) {
        _OSBASE_TRACE(1, ("--- %s CMPI CreateInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Create route.
    if(addIPRoute(nHopIP) != NH_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not add NextHopIP.");
        _OSBASE_TRACE(1, ("--- %s CMPI CreateInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }
    CMReturnObjectPath(rslt, cop);

 out:
    FREE_SAFE(nHopIP);
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI CreateInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_NextHopIPRouteProviderSetInstance(CMPIInstanceMI *mi,
                                                    const CMPIContext *ctx,
                                                    const CMPIResult *rslt,
                                                    const CMPIObjectPath *cop,
                                                    const CMPIInstance *ci,
                                                    const char **properties)
{

    _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() called", _ClassName));

    struct nextHopIP     *nHopIP = NULL;                // Route to be set.
    struct nextHopIPList *list   = NULL;                // NextHopIP list.
    CMPIData              data;                         // CIM datum to extract
    CMPIStatus            rc     = {CMPI_RC_OK, NULL};  // Return status of
                                                        // CIM operations.
                                                        // property values.

    // Alloc struct.
    nHopIP = malloc(sizeof(*nHopIP));
    if(nHopIP == NULL) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_FOUND,
                             "Cannot alloc memory for \'nHopIP\'.");
        _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }
    memset(nHopIP, 0, sizeof(*nHopIP));

    // Get instanceId.
    data = CMGetKey(cop, "InstanceId", &rc);
    if((rc.rc != CMPI_RC_OK) || (CMIsNullValue(data)) ||
        (data.type != CMPI_string)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                             "Could not get NextHopIP InstanceID.");
        _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Get route info.
    if((getIPRouteIId(&list, CMGetCharPtr(data.value.string)) != NH_OK) ||
       (list == NULL)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_FOUND,
                             "NextHopIP does not exist.");
        _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Delete route.
    if(delIPRoute(list->sptr) != NH_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not delete NextHopIP.");
        _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Get nextHopIP info.
    rc = getNextHopIPParams(ci, nHopIP, list->sptr);
    if(rc.rc != CMPI_RC_OK) {
        _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Create route.
    if(addIPRoute(nHopIP) != NH_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not add NextHopIP.");
        _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

 out:
    freeNextHopIPList(list);
    FREE_SAFE(nHopIP);
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_NextHopIPRouteProviderDeleteInstance(CMPIInstanceMI *mi,
                                                       const CMPIContext *ctx,
                                                       const CMPIResult *rslt,
                                                       const CMPIObjectPath *cop)
{
    _OSBASE_TRACE(1, ("--- %s CMPI DeleteInstance() called", _ClassName));

    CMPIData          data;                        // CIM datum to extract
                                                   // property values.
    struct nextHopIP *nHopIP = NULL;               // Route to be deleted.
    CMPIStatus        rc     = {CMPI_RC_OK, NULL}; // Return status of
                                                   // CIM operations.

    // Alloc struct.
    nHopIP = malloc(sizeof(*nHopIP));
    if(nHopIP == NULL) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_FOUND,
                             "Cannot alloc memory for \'nHopIP\'.");
        _OSBASE_TRACE(1, ("--- %s CMPI DeleteInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }
    memset(nHopIP, 0, sizeof(*nHopIP));

    // Get instanceId.
    data = CMGetKey(cop, "InstanceId", &rc);
    if((rc.rc != CMPI_RC_OK) || (CMIsNullValue(data)) ||
       (data.type != CMPI_string)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                             "Could not get NextHopIP InstanceID.");
        _OSBASE_TRACE(1, ("--- %s CMPI DeleteInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }
    snprintf(nHopIP->instanceID, 64, "%s", CMGetCharPtr(data.value.string));

    // Delete route.
    if(delIPRoute(nHopIP) != NH_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not delete NextHopIP.");
        _OSBASE_TRACE(1, ("--- %s CMPI DeleteInstance() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

 out:
    FREE_SAFE(nHopIP);
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI DeleteInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_NextHopIPRouteProviderExecQuery(CMPIInstanceMI *mi,
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
CMPIStatus OSBase_NextHopIPRouteProviderMethodCleanup(CMPIMethodMI *mi,
                                                      const CMPIContext *ctx,
                                                      CMPIBoolean terminating)
{
    _OSBASE_TRACE(1, ("--- %s CMPI MethodCleanup() called", _ClassName));

    CMReturn(CMPI_RC_OK);

    _OSBASE_TRACE(1, ("--- %s CMPI MethodCleanup() exited", _ClassName));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_NextHopIPRouteProviderInvokeMethod(CMPIMethodMI *mi,
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

    if((strcasecmp(CMGetCharPtr(class), _ClassName) == 0) &&
       (strcasecmp("method_name", methodName) == 0)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_SUPPORTED,
                             methodName);
    } else {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_FOUND,
                             methodName);
    }

 out:
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
CMInstanceMIStub(OSBase_NextHopIPRouteProvider,
                 OSBase_NextHopIPRouteProvider,
                 _broker,
                 CMNoHook);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMMethodMIStub(OSBase_NextHopIPRouteProvider,
               OSBase_NextHopIPRouteProvider,
               _broker,
               CMNoHook);
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// HELPERS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
CMPIStatus getNextHopIPParams(const CMPIInstance *ci, struct nextHopIP *nHopIP,
                              const struct nextHopIP *nHopIPPrev)
{
    _OSBASE_TRACE(3, ("--- getNextHopIPParams() called"));

    CMPIData   data;                              // CIM datum to extract
                                                  // property values.
    char       addrTypeStr[ADDR_TYPE_STR_MAXLEN]; // Address type (string)
    char       tmpStr[256];                       // Used to store strings
                                                  // temporarily.
    char       ifName[16];                        // Interface name as string.
    CMPIStatus rc = {CMPI_RC_OK, NULL};           // Return status of CIM
                                                  // operations.

    // dstAddress.
    data = CMGetProperty(ci, "DestinationAddress", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_string)) {
        snprintf(nHopIP->dstAddress, 64, "%s", CMGetCharPtr(data.value.string));
    } else {
        if(nHopIPPrev == NULL) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                                 "Cannot determine DestinationAddress");
            goto out;
        } else {
            snprintf(nHopIP->dstAddress, 64, "%s", nHopIPPrev->dstAddress);
        }
    }

    // addressType.
    data = CMGetProperty(ci, "AddressType", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_uint16)) {
        nHopIP->addressType = data.value.uint16;
        if(getAddrType(nHopIP->dstAddress) != nHopIP->addressType) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_INVALID_PARAMETER,
                                 "Invalid AddressType");
            goto out;
        }
    } else {
        if(nHopIPPrev == NULL) {
            nHopIP->addressType = getAddrType(nHopIP->dstAddress);
        } else {
            nHopIP->addressType = nHopIPPrev->addressType;
        }
    }
    if(getAddrType(nHopIP->dstAddress) == NH_AT_UNKNOWN) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_INVALID_PARAMETER,
                             "Invalid AddressType");
        goto out;
    }
    getAddrTypeStr(nHopIP->addressType, addrTypeStr);

    // dstMask.
    // @todo use x.x.x.x string instead of prefix.
    if(nHopIP->addressType == NH_AT_IPV4) {
        data = CMGetProperty(ci, "DestinationMask", &rc);
        if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
           (data.type == CMPI_string)) {
            snprintf(nHopIP->dstMask, 64, "%s",
                     CMGetCharPtr(data.value.string));
        } else {
            if(nHopIPPrev == NULL) {
                CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                                     "Cannot determine DestinationMask");
                goto out;
            } else {
                snprintf(nHopIP->dstMask, 64, "%s", nHopIPPrev->dstMask);
            }
        }
    } else {
        snprintf(nHopIP->dstMask, 64, "%d", 0);
    }

    // prefixLength.
    if(nHopIP->addressType == NH_AT_IPV6) {
        data = CMGetProperty(ci, "PrefixLength", &rc);
        if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
           (data.type == CMPI_uint16)) {
            nHopIP->prefixLength = data.value.uint16;
        } else {
            if(nHopIPPrev == NULL) {
                CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                                     "Cannot determine PrefixLength");
                goto out;
            } else {
                nHopIP->prefixLength = nHopIPPrev->prefixLength;
            }
        }
    } else {
        nHopIP->prefixLength = 0;
    }

    // adminDistance.
    data = CMGetProperty(ci, "AdminDistance", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_uint16)) {
        nHopIP->adminDistance = data.value.uint16;
    } else {
        if(nHopIPPrev == NULL) {
            // @todo Obtain from AdministrativeDistance Class.
            // Static.
            nHopIP->adminDistance = 1;
        } else {
            nHopIP->adminDistance = nHopIPPrev->adminDistance;
        }
    }

    // routeDerivation.
    data = CMGetProperty(ci, "RouteDerivation", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_uint16)) {
        nHopIP->routeDerivation = data.value.uint16;
        if((nHopIP->routeDerivation < NH_RTD_UNKNOWN) ||
           (nHopIP->routeDerivation > NH_RTD_OSPF)) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_INVALID_PARAMETER,
                                 "Invalid RouteDerivation");
            goto out;
        }
    } else {
        if(nHopIPPrev == NULL) {
            nHopIP->routeDerivation = NH_RTD_UNKNOWN;
        } else {
            nHopIP->routeDerivation = nHopIPPrev->routeDerivation;
        }
    }

    // otherDerivation.
    data = CMGetProperty(ci, "OtherDerivation", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_string)) {
        snprintf(nHopIP->otherDerivation, 64, "%s",
                 CMGetCharPtr(data.value.string));
    } else {
        if(nHopIPPrev == NULL) {
            snprintf(nHopIP->otherDerivation, 64, "%s", "");
        } else {
            snprintf(nHopIP->otherDerivation, 64, "%s",
                     nHopIPPrev->otherDerivation);
        }
    }

    // isStatic.
    data = CMGetProperty(ci, "IsStatic", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_boolean)) {
        nHopIP->isStatic = data.value.boolean;
    } else {
        if(nHopIPPrev == NULL) {
            // True.
            nHopIP->isStatic = 1;
        } else {
            nHopIP->isStatic = nHopIPPrev->isStatic;
        }
    }

    // routeMetric.
    data = CMGetProperty(ci, "RouteMetric", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_uint16)) {
        nHopIP->routeMetric = data.value.uint16;
    } else {
        if(nHopIPPrev == NULL) {
            nHopIP->routeMetric = 1;
        } else {
            nHopIP->routeMetric = nHopIPPrev->routeMetric;
        }
    }

    // typeOfRoute.
    data = CMGetProperty(ci, "TypeOfRoute", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_uint16)) {
        nHopIP->typeOfRoute = data.value.uint16;
        if((nHopIP->typeOfRoute < NH_TOR_ADMINISTRATOR) ||
           (nHopIP->typeOfRoute > NH_TOR_ACTUAL)) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_INVALID_PARAMETER,
                                 "Invalid TypeOfRoute");
            goto out;
        }
    } else {
        if(nHopIPPrev == NULL) {
            // Administrator Defined Route.
            nHopIP->typeOfRoute = 2;
        } else {
            nHopIP->typeOfRoute = nHopIPPrev->typeOfRoute;
        }
    }

    // routeGateway.
    data = CMGetProperty(ci, "RouteGateway", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_string)) {
        snprintf(nHopIP->routeGateway, 64, "%s",
                 CMGetCharPtr(data.value.string));
    } else {
        if(nHopIPPrev == NULL) {
            snprintf(nHopIP->routeGateway, 64, "%s", "");
        } else {
            snprintf(nHopIP->routeGateway, 64, "%s", nHopIPPrev->routeGateway);
        }
    }

    // routeOutputIf.
    data = CMGetProperty(ci, "RouteOutputIf", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_uint16)) {
        nHopIP->routeOutputIf = data.value.uint16;
    } else {
        if(nHopIPPrev == NULL) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NO_SUCH_PROPERTY,
                                 "Cannot determine RouteOutputIf");
            goto out;
        } else {
            nHopIP->routeOutputIf = nHopIPPrev->routeOutputIf;
        }
    }
    if_indextoname(nHopIP->routeOutputIf, ifName);
    if(ifName == NULL || (strlen(ifName) == 0)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Specified interface do not exist");
        goto out;
    }

    // routeScope.
    data = CMGetProperty(ci, "RouteScope", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_uint16)) {
        nHopIP->routeScope = data.value.uint16;
        if((nHopIP->routeScope < NH_RTS_UNIVERSE) ||
           (nHopIP->routeScope > NH_RTS_NOWHERE)) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_INVALID_PARAMETER,
                                 "Invalid RouteScope");
            goto out;
        }
    } else {
        if(nHopIPPrev == NULL) {
            nHopIP->routeScope = NH_RTS_UNIVERSE;
        } else {
            nHopIP->routeScope = nHopIPPrev->routeScope;
        }
    }

    // routeTable.
    data = CMGetProperty(ci, "RouteTable", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_uint16)) {
        nHopIP->routeTable = data.value.uint16;
        if((nHopIP->routeTable < NH_RTT_UNSPEC) ||
           (nHopIP->routeTable > NH_RTT_LOCAL)) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_INVALID_PARAMETER,
                                 "Invalid RouteTable");
            goto out;
        }
    } else {
        if(nHopIPPrev == NULL) {
            nHopIP->routeTable = NH_RTT_MAIN;
        } else {
            nHopIP->routeTable = nHopIPPrev->routeTable;
        }
    }

    // routeType.
    data = CMGetProperty(ci, "RouteType", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_uint16)) {
        nHopIP->routeType = data.value.uint16;
        if((nHopIP->routeType < NH_RTTY_UNSPEC) ||
           (nHopIP->routeType > NH_RTTY_XRESOLVE)) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_INVALID_PARAMETER,
                                 "Invalid RouteType");
            goto out;
        }
    } else {
        if(nHopIPPrev == NULL) {
            nHopIP->routeType = NH_RTTY_UNICAST;
        } else {
            nHopIP->routeType = nHopIPPrev->routeType;
        }
    }

    // caption.
    data = CMGetProperty(ci, "Caption", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_string)) {
        snprintf(nHopIP->caption, 64, "%s", CMGetCharPtr(data.value.string));
    } else {
        if(nHopIPPrev == NULL) {
            snprintf(nHopIP->caption, 64, "NextHop %s route.", addrTypeStr);
        } else {
            snprintf(nHopIP->caption, 64, "%s", nHopIPPrev->caption);
        }
    }

    // description.
    data = CMGetProperty(ci, "Description", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_string)) {
        snprintf(nHopIP->description, 256, "%s",
                 CMGetCharPtr(data.value.string));
    } else {
        if(nHopIPPrev == NULL) {
            if(nHopIP->addressType == NH_AT_IPV6) {
                // IPv6.
                snprintf(nHopIP->description, 256,
                         "NextHop to %s/%d through \"%s\" interface.",
                         nHopIP->dstAddress, nHopIP->prefixLength, ifName);
            } else {
                // IPv4.
                snprintf(nHopIP->description, 256,
                         "NextHop to %s/%d through \"%s\" interface.",
                         nHopIP->dstAddress, nHopIP->dstMask, ifName);
            }
        } else {
            snprintf(nHopIP->description, 256, "%s", nHopIPPrev->description);
        }
    }

    // elementName.
    data = CMGetProperty(ci, "ElementName", &rc);
    if((rc.rc == CMPI_RC_OK) && (!CMIsNullValue(data)) &&
       (data.type == CMPI_string)) {
        snprintf(nHopIP->elementName, 64, "%s",
                 CMGetCharPtr(data.value.string));
    } else {
        if(nHopIPPrev == NULL) {
            if(nHopIP->addressType == NH_AT_IPV6) {
                // IPv6.
                snprintf(nHopIP->elementName, 64, "%s-%s/%d",
                         addrTypeStr, nHopIP->dstAddress, nHopIP->prefixLength);
            } else {
                // IPv4.
                snprintf(nHopIP->elementName, 64, "%s-%s/%d",
                         addrTypeStr, nHopIP->dstAddress, nHopIP->dstMask);
            }
        } else {
            snprintf(nHopIP->elementName, 64, "%s", nHopIPPrev->elementName);
        }
    }

    // instanceID.
    // This parameter must be autogenerated.
    gethostname(tmpStr, MAXHOSTNAMELEN);
    if(nHopIP->addressType == NH_AT_IPV6) {
        snprintf(nHopIP->instanceID, 64, INSTANCEID_FORMAT,
                 tmpStr, nHopIP->dstAddress, nHopIP->prefixLength,
                 nHopIP->routeTable, nHopIP->routeOutputIf,
                 nHopIP->routeScope, nHopIP->routeGateway);
    } else {
        snprintf(nHopIP->instanceID, 64, INSTANCEID_FORMAT,
                 tmpStr, nHopIP->dstAddress, nHopIP->dstMask,
                 nHopIP->routeTable, nHopIP->routeOutputIf,
                 nHopIP->routeScope, nHopIP->routeGateway);
    }

 out:
    _OSBASE_TRACE(3, ("--- getNextHopIPParams() exited"));
    return rc;
}
//------------------------------------------------------------------------------
