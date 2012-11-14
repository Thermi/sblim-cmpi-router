#ifndef _CMPIOSBASE_LANENDPOINT_H_
#define _CMPIOSBASE_LANENDPOINT_H_

/**
 * @file
 * cmpiOSBase_LANEndpoint.h
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
 * CIM class Linux_LANEndpoint.
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 */

#include "cmpidt.h"
#include "OSBase_LANEndpoint.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * Provider ClassName.
 */
static char *_ClassName = "Linux_LANEndpoint";

/**
 * Method to create a CMPIObjectPath of this class.
 * @param _broker [in] CIM Object Manager.
 * @param ctx [in] Context object.
 * @param cop [in] Source objectPath.
 * @param LANEP [in] LANEndpoint instance.
 * @param rc [in] cim status.
 * @return created LANEndpoint ObjectPath.
 */
CMPIObjectPath* _makePath_LANEndpoint(const CMPIBroker *_broker,
                                      const CMPIContext *ctx,
                                      const CMPIObjectPath *cop,
                                      const struct LANEndpoint *LANEP,
                                      CMPIStatus *rc);

/**
 * Method to create a CMPIInstance of this class.
 * @param _broker [in] CIM Object Manager.
 * @param ctx [in] Context object.
 * @param cop [in] Source objectPath.
 * @param properties [in]
 * @param LANEP [in] LANEndpoint instance.
 * @param rc [in] cim status.
 * @return created LANEndpoint instance.
 */
CMPIInstance* _makeInst_LANEndpoint(const CMPIBroker *_broker,
                                    const CMPIContext *ctx,
                                    const CMPIObjectPath *cop,
                                    const char **properties,
                                    const struct LANEndpoint *LANEP,
                                    CMPIStatus *rc);

///////////////////////////////////////////////////////////////////////////////

#endif //_CMPIOSBASE_LANENDPOINT_H_
