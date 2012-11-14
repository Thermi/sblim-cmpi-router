#ifndef _CMPIOSBASE_NEXTHOPIPROUTE_H_
#define _CMPIOSBASE_NEXTHOPIPROUTE_H_

/**
 * @file
 * cmpiOSBase_NextHopIPRoute.h
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
 * This file defines the interfaces for the factory implementation of the
 * CIM class Linux_NextHopIPRoute.
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 */

#include "cmpidt.h"
#include "OSBase_NextHopIPRoute.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * Provider ClassName.
 */
static char *_ClassName = "Linux_NextHopIPRoute";

/**
 * Method to create a CMPIObjectPath of this class.
 * @param _broker [in] CIM Object Manager.
 * @param ctx [in] Context object.
 * @param cop [in] Source objectPath.
 * @param NHop [in] nextHopIP instance.
 * @param rc [in] cim status.
 * @return created NextHopIPRoute ObjectPath.
 */
CMPIObjectPath* _makePath_NextHopIPRoute(const CMPIBroker *_broker,
                                         const CMPIContext *ctx,
                                         const CMPIObjectPath *cop,
                                         const struct nextHopIP *NHop,
                                         CMPIStatus *rc);

/**
 * Method to create a CMPIInstance of this class.
 * @param _broker [in] CIM Object Manager.
 * @param ctx [in] Context object.
 * @param cop [in] Source objectPath.
 * @param properties [in]
 * @param NHop [in] nextHopIP instance.
 * @param rc [in] cim status.
 * @return created NextHopIPRoute instance.
 */
CMPIInstance* _makeInst_NextHopIPRoute(const CMPIBroker *_broker,
                                       const CMPIContext *ctx,
                                       const CMPIObjectPath *cop,
                                       const char **properties,
                                       const struct nextHopIP *NHop,
                                       CMPIStatus *rc);

///////////////////////////////////////////////////////////////////////////////

#endif //_CMPIOSBASE_NEXTHOPIPROUTE_H_
