/**
 * @file
 * cmpiOSBase_RouteUsesEndpointProvider.c
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Eclipse Public License from
 * http://www.opensource.org/licenses/eclipse-1.0.php
 *
 * @author       Federico Martin Casares (warptrosse@gmail.com)
 *
 * @contributors
 *
 * @description Linux_RouteUsesEndpointProvider association provider
 *              implementation.
 *              The following CMPI instance methods are supported:
 *              - Cleanup
 *              - EnumerateInstanceNames
 *              - EnumerateInstances
 *              - GetInstance
 *              - AssociationCleanup
 *              - Associators
 *              - AssociatorNames
 *              - References
 *              - ReferenceNames
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
#include "cmpiOSBase_RouteUsesEndpoint.h"

//------------------------------------------------------------------------------
static const CMPIBroker *_broker;

#ifdef CMPI_VER_100
#define OSBase_RouteUsesEndpointProviderSetInstance OSBase_RouteUsesEndpointProviderModifyInstance
#endif //CMPI_VER_100
//------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
// INSTANCE PROVIDER INTERFACE
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderCleanup(CMPIInstanceMI *mi,
                                                   const CMPIContext *ctx,
                                                   CMPIBoolean terminating)
{
    _OSBASE_TRACE(1, ("--- %s CMPI Cleanup() called", _ClassName));

    CMReturn(CMPI_RC_OK);

    _OSBASE_TRACE(1, ("--- %s CMPI Cleanup() exited", _ClassName));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderEnumInstanceNames(CMPIInstanceMI *mi,
                                                             const CMPIContext *ctx,
                                                             const CMPIResult *rslt,
                                                             const CMPIObjectPath *ref)
{
    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() called", _ClassName));

    CMPIStatus rc    = {CMPI_RC_OK, NULL}; // Return status of CIM operations.
    int        cmdrc = 0;                  // To handle errors received
                                           // from external functions.

    // Get association instances 1toN.
    cmdrc = _assoc_create_inst_1toN(_broker, ctx, rslt, ref, _ClassName,
                                    _RefLeftClass, _RefRightClass,
                                    _RefLeft, _RefRight, 1, 0, &rc);
    if(cmdrc != 0) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not get association instances.");
        if(rc.msg != NULL) {
            _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
        } else {
            _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() failed",
                              _ClassName));
        }
        goto out;
    }

 out:
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstanceNames() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderEnumInstances(CMPIInstanceMI *mi,
                                                         const CMPIContext *ctx,
                                                         const CMPIResult *rslt,
                                                         const CMPIObjectPath *ref,
                                                         const char **properties)
{
    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() called", _ClassName));

    CMPIStatus rc    = {CMPI_RC_OK, NULL}; // Return status of CIM operations.
    int        cmdrc = 0;                  // To handle errors received
                                           // from external functions.
    // Get association instances 1toN.
    cmdrc = _assoc_create_inst_1toN(_broker, ctx, rslt, ref, _ClassName,
                                    _RefLeftClass, _RefRightClass,
                                    _RefLeft, _RefRight, 1, 1, &rc);
    if(cmdrc != 0) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not get association instances.");
        if(rc.msg != NULL) {
            _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
        } else {
            _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() failed",
                              _ClassName));
        }
        goto out;
    }

 out:
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI EnumInstances() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderGetInstance(CMPIInstanceMI *mi,
                                                       const CMPIContext *ctx,
                                                       const CMPIResult *rslt,
                                                       const CMPIObjectPath *cop,
                                                       const char **properties)
{
    _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() called", _ClassName));

    CMPIStatus    rc = {CMPI_RC_OK, NULL}; // Return status of CIM operations.
    CMPIInstance *ci = NULL;               // Class instance.

    // Get association instance.
    ci = _assoc_get_inst(_broker, ctx, cop, _ClassName, _RefLeft,
                         _RefRight, &rc);
    if(ci == NULL) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not get association instance.");
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
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI GetInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderCreateInstance(CMPIInstanceMI *mi,
                                                          const CMPIContext *ctx,
                                                          const CMPIResult *rslt,
                                                          const CMPIObjectPath *cop,
                                                          const CMPIInstance *ci)
{
    _OSBASE_TRACE(1, ("--- %s CMPI CreateInstance() called", _ClassName));

    CMPIStatus rc = {CMPI_RC_OK, NULL}; // Return status of CIM operations.

    CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_SUPPORTED,
                         "CIM_ERR_NOT_SUPPORTED" );
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI CreateInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderSetInstance(CMPIInstanceMI *mi,
                                                       const CMPIContext *ctx,
                                                       const CMPIResult *rslt,
                                                       const CMPIObjectPath *cop,
                                                       const CMPIInstance *ci,
                                                       const char **properties)
{

    _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() called", _ClassName));

    CMPIStatus rc = {CMPI_RC_OK, NULL}; // Return status of CIM operations.

    CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_SUPPORTED,
                         "CIM_ERR_NOT_SUPPORTED" );
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI SetInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderDeleteInstance(CMPIInstanceMI *mi,
                                                          const CMPIContext *ctx,
                                                          const CMPIResult *rslt,
                                                          const CMPIObjectPath *cop)
{
    _OSBASE_TRACE(1, ("--- %s CMPI DeleteInstance() called", _ClassName));

    CMPIStatus rc = {CMPI_RC_OK, NULL}; // Return status of CIM operations.

    CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_SUPPORTED,
                         "CIM_ERR_NOT_SUPPORTED" );
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI DeleteInstance() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderExecQuery(CMPIInstanceMI *mi,
                                                     const CMPIContext *ctx,
                                                     const CMPIResult *rslt,
                                                     const CMPIObjectPath *ref,
                                                     const char *lang,
                                                     const char *query)
{
    _OSBASE_TRACE(1, ("--- %s CMPI ExecQuery() called", _ClassName));

    CMPIStatus rc = {CMPI_RC_OK, NULL}; // Return status of CIM operations.

    CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_NOT_SUPPORTED,
                         "CIM_ERR_NOT_SUPPORTED" );
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI ExecQuery() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// ASSOCIATOR PROVIDER INTERFACE
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderAssociationCleanup(CMPIAssociationMI *mi,
                                                              const CMPIContext *ctx,
                                                              CMPIBoolean terminating)
{
    _OSBASE_TRACE(1, ("--- %s CMPI AssociationCleanup() called", _ClassName));

    CMReturn(CMPI_RC_OK);

    _OSBASE_TRACE(1, ("--- %s CMPI AssociationCleanup() exited", _ClassName));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderAssociators(CMPIAssociationMI *mi,
                                                       const CMPIContext *ctx,
                                                       const CMPIResult *rslt,
                                                       const CMPIObjectPath *cop,
                                                       const char *assocClass,
                                                       const char *resultClass,
                                                       const char *role,
                                                       const char *resultRole,
                                                       const char **propertyList)
{
    _OSBASE_TRACE(1, ("--- %s CMPI Associators() called", _ClassName));

    CMPIObjectPath *op   = NULL;               // Object path instance
                                               // (assocClass).
    CMPIObjectPath *rcop = NULL;               // Object path instance
                                               // (resultClass).
    CMPIStatus      rc   = {CMPI_RC_OK, NULL}; // Return status of CIM
                                               // operations.

    // Creating objectPath of this class.
    if(assocClass) {
        op = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(cop, &rc)),
                             _ClassName, &rc);
        if(op == NULL) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                 "Create CMPIObjectPath failed (assocClass).");
            _OSBASE_TRACE(2, ("--- %s CMPI Associators() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
            goto out;
        }
    }

    // If "assocClass" is defined and "op" is not of type "assocClass".
    if((assocClass != NULL) &&
       (CMClassPathIsA(_broker, op, assocClass, &rc) == 0)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Invalid assocClass.");
        _OSBASE_TRACE(2, ("--- %s CMPI Associatoners() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Check parameters.
    if(_assoc_check_parameter_const(_broker, cop, _RefLeft, _RefRight,
                                    _RefLeftClass, _RefRightClass,
                                    resultClass, role, resultRole,
                                    &rc) == 0) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Invalid parameters: association is not "  \
                             "responsible for this request");
        _OSBASE_TRACE(2, ("--- %s CMPI Associators() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Creating resultClass objectPath.
    if(resultClass) {
        rcop = CMNewObjectPath(_broker,
                               CMGetCharPtr(CMGetNameSpace(cop, &rc)),
                               resultClass, &rc);
        if(rcop == NULL) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                 "Create CMPIObjectPath failed "        \
                                 "(resultClass).");
            _OSBASE_TRACE(2, ("--- %s CMPI Associators() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
            goto out;
        }

        // If "rcop" is not of type "_RefRightClass" and "_RefLeftClass".
        if((CMClassPathIsA(_broker, rcop, _RefRightClass, &rc) == 0) &&
           (CMClassPathIsA(_broker, rcop, _RefLeftClass, &rc) == 0)) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                 "Invalid resultClass.");
            _OSBASE_TRACE(2, ("--- %s CMPI Associators() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
            goto out;
        }
    }

    // Get assoc instances.
    rc = _assoc_RouteUsesEndpoint(_broker, ctx, rslt, cop, resultClass,
                                  ATYPE_ASSOC, &rc);
    if(rc.rc != CMPI_RC_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not get association instances.");
        if(rc.msg != NULL)  {
            _OSBASE_TRACE(1, ("--- %s CMPI Associators() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
        } else {
            _OSBASE_TRACE(1, ("--- %s CMPI Associators() failed.",
                              _ClassName));
        }
        goto out;
    }

 out:
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI Associators() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderAssociatorNames(CMPIAssociationMI *mi,
                                                           const CMPIContext *ctx,
                                                           const CMPIResult *rslt,
                                                           const CMPIObjectPath *cop,
                                                           const char *assocClass,
                                                           const char *resultClass,
                                                           const char *role,
                                                           const char *resultRole)
{
    _OSBASE_TRACE(1, ("--- %s CMPI AssociatorNames() called", _ClassName));

    CMPIObjectPath *op   = NULL;               // Object path instance
                                               // (assocClass).
    CMPIObjectPath *rcop = NULL;               // Object path instance
                                               // (resultClass).
    CMPIStatus      rc   = {CMPI_RC_OK, NULL}; // Return status of CIM
                                               // operations.

    // Creating assocClass objectPath.
    if(assocClass) {
        op = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(cop, &rc)),
                             _ClassName, &rc);
        if(op == NULL) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                 "Create CMPIObjectPath failed (assocClass).");
            _OSBASE_TRACE(2, ("--- %s CMPI AssociatorNames() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
            goto out;
        }
    }

    // If "assocClass" is defined and "op" is not of type "assocClass".
    if((assocClass != NULL) &&
       (CMClassPathIsA(_broker, op, assocClass, &rc) == 0)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Invalid assocClass.");
        _OSBASE_TRACE(2, ("--- %s CMPI AssociatonerNames() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Check parameters.
    if(_assoc_check_parameter_const(_broker, cop, _RefLeft, _RefRight,
                                    _RefLeftClass, _RefRightClass,
                                    resultClass, role, resultRole,
                                    &rc) == 0) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Invalid parameters: association is not "  \
                             "responsible for this request");
        _OSBASE_TRACE(2, ("--- %s CMPI AssociatorNames() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Creating resultClass objectPath.
    if(resultClass) {
        rcop = CMNewObjectPath(_broker,
                               CMGetCharPtr(CMGetNameSpace(cop, &rc)),
                               resultClass, &rc);
        if(rcop == NULL) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                 "Create CMPIObjectPath failed "        \
                                 "(resultClass).");
            _OSBASE_TRACE(2, ("--- %s CMPI AssociatorNames() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
            goto out;
        }

        // If "rcop" is not of type "_RefRightClass" and "_RefLeftClass".
        if((CMClassPathIsA(_broker, rcop, _RefRightClass, &rc) == 0) &&
           (CMClassPathIsA(_broker, rcop, _RefLeftClass, &rc) == 0)) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                 "Invalid resultClass.");
            _OSBASE_TRACE(2, ("--- %s CMPI AssociatorNames() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
            goto out;
        }
    }

    // Get assoc objectPaths.
    rc = _assoc_RouteUsesEndpoint(_broker, ctx, rslt, cop, resultClass,
                                  ATYPE_ASSOCN, &rc);
    if(rc.rc != CMPI_RC_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not get association instances.");
        if(rc.msg != NULL)  {
            _OSBASE_TRACE(1, ("--- %s CMPI AssociatorNames() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
        } else {
            _OSBASE_TRACE(1, ("--- %s CMPI AssociatorNames() failed",
                              _ClassName));
        }
        goto out;
    }

 out:
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI AssociatorNames() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderReferences(CMPIAssociationMI *mi,
                                                      const CMPIContext *ctx,
                                                      const CMPIResult *rslt,
                                                      const CMPIObjectPath *cop,
                                                      const char *assocClass,
                                                      const char *role,
                                                      const char **propertyList)
{
    _OSBASE_TRACE(1, ("--- %s CMPI References() called", _ClassName));

    CMPIObjectPath *op = NULL;               // Object path instance
                                             // (assocClass).
    CMPIStatus      rc = {CMPI_RC_OK, NULL}; // Return status of CIM operations.

    // Creating assocClass objectPath.
    if(assocClass) {
        op = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(cop, &rc)),
                             _ClassName, &rc);
        if(op == NULL) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                 "Create CMPIObjectPath failed (assocClass).");
            _OSBASE_TRACE(2, ("--- %s CMPI References() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
            goto out;
        }
    }

    // If "assocClass" is defined and "op" is not of type "assocClass".
    if((assocClass != NULL) &&
       (CMClassPathIsA(_broker, op, assocClass, &rc) == 0)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Invalid assocClass.");
        _OSBASE_TRACE(2, ("--- %s CMPI References() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Check parameters.
    if(_assoc_check_parameter_const(_broker, cop, _RefLeft, _RefRight,
                                    _RefLeftClass, _RefRightClass,
                                    NULL, role, NULL, &rc) == 0) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Invalid parameters: association is not " \
                             "responsible for this request");
        _OSBASE_TRACE(2, ("--- %s CMPI References() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Get references instances.
    rc = _assoc_RouteUsesEndpoint(_broker, ctx, rslt, cop, assocClass,
                                  ATYPE_REFER, &rc);
    if(rc.rc != CMPI_RC_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not get association instances.");
        if(rc.msg != NULL)  {
            _OSBASE_TRACE(1, ("--- %s CMPI References() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
        } else {
            _OSBASE_TRACE(1, ("--- %s CMPI References() failed",
                              _ClassName));
        }
        goto out;
    }

 out:
    CMReturnDone(rslt);
    _OSBASE_TRACE(1, ("--- %s CMPI References() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMPIStatus OSBase_RouteUsesEndpointProviderReferenceNames(CMPIAssociationMI *mi,
                                                          const CMPIContext *ctx,
                                                          const CMPIResult *rslt,
                                                          const CMPIObjectPath *cop,
                                                          const char *assocClass,
                                                          const char *role)
{
    _OSBASE_TRACE(1, ("--- %s CMPI ReferenceNames() called", _ClassName));

    CMPIObjectPath *op = NULL;               // Object path instance
                                             // (assocClass).
    CMPIStatus      rc = {CMPI_RC_OK, NULL}; // Return status of CIM operations.

    // Creating assocClass objectPath.
    if(assocClass) {
        op = CMNewObjectPath(_broker, CMGetCharPtr(CMGetNameSpace(cop, &rc)),
                             _ClassName, &rc);
        if(op == NULL) {
            CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                                 "Create CMPIObjectPath failed (assocClass).");
            _OSBASE_TRACE(2, ("--- %s CMPI ReferenceNames() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
            goto out;
        }
    }

    // If "assocClass" is defined and "op" is not of type "assocClass".
    if((assocClass != NULL) &&
       (CMClassPathIsA(_broker, op, assocClass, &rc) == 0)) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Invalid assocClass.");
        _OSBASE_TRACE(2, ("--- %s CMPI ReferenceNames() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Check parameters.
    if(_assoc_check_parameter_const(_broker, cop, _RefLeft, _RefRight,
                                    _RefLeftClass, _RefRightClass,
                                    NULL, role, NULL, &rc) == 0) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Invalid parameters: association is not " \
                             "responsible for this request");
        _OSBASE_TRACE(2, ("--- %s CMPI ReferenceNames() failed: %s",
                          _ClassName, CMGetCharPtr(rc.msg)));
        goto out;
    }

    // Get references objectPaths.
    rc = _assoc_RouteUsesEndpoint(_broker, ctx, rslt, cop, assocClass,
                                  ATYPE_REFERN, &rc);
    if(rc.rc != CMPI_RC_OK) {
        CMSetStatusWithChars(_broker, &rc, CMPI_RC_ERR_FAILED,
                             "Could not get association instances.");
        if(rc.msg != NULL)  {
            _OSBASE_TRACE(1, ("--- %s CMPI ReferenceNameNames() failed: %s",
                              _ClassName, CMGetCharPtr(rc.msg)));
        } else {
            _OSBASE_TRACE(1, ("--- %s CMPI ReferenceNameNames() failed",
                              _ClassName));
        }
        goto out;
    }

 out:
    CMReturnDone(rslt);

    _OSBASE_TRACE(1, ("--- %s CMPI ReferenceNames() exited", _ClassName));
    return rc;
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROVIDER FACTORY
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
CMInstanceMIStub(OSBase_RouteUsesEndpointProvider,
                 OSBase_RouteUsesEndpointProvider,
                 _broker,
                 CMNoHook);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CMAssociationMIStub(OSBase_RouteUsesEndpointProvider,
                    OSBase_RouteUsesEndpointProvider,
                    _broker,
                    CMNoHook);
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
