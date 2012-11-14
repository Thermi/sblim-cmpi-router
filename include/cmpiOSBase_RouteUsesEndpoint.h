#ifndef _CMPIOSBASE_ROUTEUSESENDPOINT_H_
#define _CMPIOSBASE_ROUTEUSESENDPOINT_H_

/**
 * @file
 * cmpiOSBase_RouteUsesEndpoint.h
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

#include "cmpidt.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * Association type.
 */
enum {
    ATYPE_ASSOC,  /**< Associators. */
    ATYPE_ASSOCN, /**< AssociatorNames. */
    ATYPE_REFER,  /**< References. */
    ATYPE_REFERN  /**< ReferenceNames. */
};

/**
 * Class name.
 */
static char *_ClassName = "Linux_RouteUsesEndpoint";

/**
 * Left reference identifier.
 */
static char *_RefLeft = "Antecedent";

/**
 * Right reference identifier.
 */
static char *_RefRight = "Dependent";

/**
 * Left reference class.
 */
static char *_RefLeftClass = "CIM_ProtocolEndpoint";

/**
 * Right reference class.
 */
static char *_RefRightClass = "CIM_NextHopRoute";

/**
 * Left reference classes.
 */
static char *_RefLeftClasses[] = { "Linux_LANEndpoint" };

/**
 * Right reference classes.
 */
static char *_RefRightClasses[] = { "Linux_NextHopIPRoute" };

/**
 * Method to create a CMPIObjectPath of this class.
 * @param _broker [in] CIM Object Manager.
 * @param ops [in] Source objectPath.
 * @param opt [in] Target objectPath.
 * @param rc [in] cim status.
 * @return created ObjectPath.
 */
CMPIObjectPath* _makePath_RouteUsesEndpoint(const CMPIBroker *_broker,
                                            const CMPIObjectPath *ops,
                                            const CMPIObjectPath *opt,
                                            CMPIStatus *rc);

/**
 * Method to create a CMPIInstance of this class.
 * @param _broker [in] CIM Object Manager.
 * @param ops [in] Source objectPath.
 * @param opt [in] Target objectPath.
 * @param rc [in] cim status.
 * @return created Instance.
 */
CMPIInstance* _makeInst_RouteUsesEndpoint(const CMPIBroker *_broker,
                                          const CMPIObjectPath *ops,
                                          const CMPIObjectPath *opt,
                                          CMPIStatus *rc);

/**
 * Retrieve a list of instances from target class (CIM_NextHopRoute
 * subclass), associated to source class.
 * @param _broker [in] CIM Object Manager.
 * @param ctx [in] context object.
 * @param rslt [in] result.
 * @param ref [in] source objectPath.
 * @param sourceInst [in] source class instance.
 * @param targetClass [in] target class name.
 * @param assocType [in] association type.
 * @param rc [in] cim status.
 * @return cim status.
 */
CMPIStatus* _assoc_get_NextHopRoute_insts(const CMPIBroker *_broker,
                                          const CMPIContext *ctx,
                                          const CMPIResult *rslt,
                                          const CMPIObjectPath *ref,
                                          const CMPIInstance *sourceInst,
                                          const char *targetClass,
                                          int assocType,
                                          CMPIStatus *rc);

/**
 * Retrieve a list of instances from target class (CIM_ProtocolEndpoint
 * subclass), associated to source class.
 * @param _broker [in] CIM Object Manager.
 * @param ctx [in] context object.
 * @param rslt [in] result.
 * @param ref [in] source objectPath.
 * @param sourceInst [in] source class instance.
 * @param targetClass [in] target class name.
 * @param assocType [in] association type.
 * @param rc [in] cim status.
 * @return cim status.
 */
CMPIStatus* _assoc_get_ProtocolEndpoint_insts(const CMPIBroker *_broker,
                                              const CMPIContext *ctx,
                                              const CMPIResult *rslt,
                                              const CMPIObjectPath *ref,
                                              const CMPIInstance *sourceInst,
                                              const char *targetClass,
                                              int assocType,
                                              CMPIStatus *rc);

/**
 * Create CMPIInstances of association.
 * @param _broker [in] CIM Object Manager.
 * @param ctx [in] context object.
 * @param rslt [in] result.
 * @param ref [in] source objectPath.
 * @param targetClass [in] target class name.
 * @param assocType [in] association type.
 * @param rc [in] cim status.
 * @return cim status.
 */
CMPIStatus _assoc_RouteUsesEndpoint(const CMPIBroker *_broker,
                                    const CMPIContext *ctx,
                                    const CMPIResult *rslt,
                                    const CMPIObjectPath *ref,
                                    const char *targetClass,
                                    int assocType,
                                    CMPIStatus *rc);

///////////////////////////////////////////////////////////////////////////////

#endif //_CMPIOSBASE_ROUTEUSESENDPOINT_H_
