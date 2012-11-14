#ifndef _OSBASE_LANENDPOINT_H_
#define _OSBASE_LANENDPOINT_H_

/**
 * @file
 * OSBase_LANEndpoint.h
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
 * This file defines the interfaces for the resource access implementation
 * of the CIM class Linux_LANEndpoint.
 */

#include "OSBase_Netlink.h"

#ifdef __cplusplus
extern "C" {
#endif

    ////////////////////////////////////////////////////////////////////////////
    // LAN ENDPOINT
    ////////////////////////////////////////////////////////////////////////////

#define LEP_OK   0 /**< Operation executed succesful. */
#define LEP_FAIL 1 /**< An error occurs when executing desired operation. */

    /**
     * Maximum hostName length.
     */
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif //MAXHOSTNAMELEN

    /**
     * CIM - LANEndpoint caption.
     */
#define LANENDPOINT_CAPTION "LAN EndPoint"

    /**
     * CIM - Creation class name.
     */
#define CREATION_CLASS_NAME "Linux_LANEndpoint"

    /**
     * CIM - LANEndpoint desctiption.
     */
#define LANENDPOINT_DESC "A communication endpoint which, when "        \
    "its associated interface device is connected to a LAN, "           \
    "may send and receive data frames."

    /**
     * CIM - EnabledState.
     */
    enum {
        LEP_ES_UNKNOWN,             /**< Unknown state. */
        LEP_ES_OTHER,               /**< Admin specified state. */
        LEP_ES_ENABLED,             /**< Indicates that the element is or could
                                       be executing commands, will process any
                                       queued commands, and queues new
                                       requests. */
        LEP_ES_DISABLED,            /**< Indicates that the element will not
                                       execute commands and will drop any new
                                       requests. */
        LEP_ES_SHUTTING_DOWN,       /**< Indicates that the element is in the
                                       process of going to a Disabled state. */
        LEP_ES_NOT_APPLICABLE,      /**< Indicates the element does not support
                                       being enabled or disabled. */
        LEP_ES_ENABLED_BUT_OFFLINE, /**< Indicates that the element might be
                                       completing commands, and will drop any
                                       new requests. */
        LEP_ES_IN_TEST,             /**< Indicates that the element is in a
                                       test state. */
        LEP_ES_DEFERRED,            /**< Indicates that the element might be
                                       completing commands, but will queue any
                                       new requests. */
        LEP_ES_QUIESCE,             /**< Indicates that the element is enabled
                                       but in a restricted mode. */
        LEP_ES_STARTING             /**< Indicates that the element is in the
                                       process of going to an Enabled state. */
    };

    /**
     * CIM - EnabledDefault.
     */
    enum {
        LEP_ED_ENABLED             = 2, /**< (Default) Indicates that the
                                           element is or could be executing
                                           commands, will process any queued
                                           commands, and queues new requests. */
        LEP_ED_DISABLED,                /**< Indicates that the element will not
                                           execute commands and will drop any
                                           new requests. */
        LEP_ED_NOT_APPLICABLE      = 5, /**< Indicates the element does not
                                           support being enabled or disabled. */
        LEP_ED_ENABLED_BUT_OFFLINE,     /**< Indicates that the element might be
                                           completing commands, and will drop
                                           any new requests. */
        LEP_ED_NO_DEFAULT,              /**< Not default enabled state. */
        LEP_ED_QUIESCE             = 9  /**< Indicates that the element is
                                           enabled but in a restricted mode. */
    };

    /**
     * CIM - RequestedState & TransitioningToState.
     */
    enum {
        LEP_RS_UNKNOWN,            /**< Indicates the last requested state for
                                      the element is unknown. */
        LEP_RS_ENABLED        = 2, /**< Enabled. */
        LEP_RS_DISABLED,           /**< Disabled. */
        LEP_RS_SHUT_DOWN,          /**< Shut down. */
        LEP_RS_NO_CHANGE,          /**< Deprecated. */
        LEP_RS_OFFLINE,            /**< Change to "Enabled but Offline"
                                      status. */
        LEP_RS_TEST,               /**< Test mode. */
        LEP_RS_DEFERRED,           /**< Dferred mode. */
        LEP_RS_QUIESCE,            /**< Quiesce mode. */
        LEP_RS_REBOOT,             /**< Reboot refers to doing a "Shut Down" and
                                      then moving to an "Enabled" state. */
        LEP_RS_RESET,              /**< Reset indicates that the element is
                                      first "Disabled" and then "Enabled". */
        LEP_RS_NOT_APPLICABLE      /**< Not applicable. */
    };

    /**
     * CIM - CommunicationStatus.
     */
    enum {
        LEP_CS_UNKNOWN,            /**< Indicates the implementation is in
                                      general capable of returning this
                                      property, but is unable to do so at this
                                      time. */
        LEP_CS_NOT_AVAILABLE,      /**< Indicates that the implementation is
                                      capable of returning a value for this
                                      property, but not ever for this particular
                                      piece of hardware/software. */
        LEP_CS_COMMUNICATION_OK,   /**< Indicates communication is established
                                      with the element, but does not convey any
                                      quality of service. */
        LEP_CS_LOST_COMMUNICATION, /**< Indicates that the Managed Element is
                                      known to exist and has been contacted
                                      successfully in the past, but is currently
                                      unreachable. */
        LEP_CS_NO_CONTACT          /**< Indicates that the monitoring system has
                                      knowledge of this element, but has never
                                      been able to establish communications
                                      with it. */
    };

    /**
     * CIM - DetailedStatus.
     */
    enum {
        LEP_DS_NOT_AVAILABLE,              /**< Indicates that the
                                              implementation is capable of
                                              returning a value for this
                                              property, but not ever for this
                                              particular piece of
                                              hardware/software. */
        LEP_DS_NO_ADDITIONAL_INFORMATION,  /**< Indicates that the element is
                                              functioning normally as indicated
                                              by PrimaryStatus = "OK". */
        LEP_DS_STRESSED,                   /**< Indicates that the element is
                                              functioning, but needs
                                              attention. */
        LEP_DS_PREDICTIVE_FAILURE,         /**< Indicates that an element is
                                              functioning normally but a failure
                                              is predicted in the near
                                              future. */
        LEP_DS_NON_RECOVERABLE_ERROR,      /**< Indicates that this element is
                                              in an error condition that
                                              requires human intervention. */
        LEP_DS_SUPPORTING_ENTITY_IN_ERROR, /**< Indicates that this element
                                              might be "OK" but that another
                                              element, on which it is dependent,
                                              is in error. */
    };

    /**
     * CIM - HealthState.
     */
    enum {
        LEP_HS_UNKNOWN               = 0,  /**< The implementation cannot report
                                              on HealthState at this time. */
        LEP_HS_OK                    = 5,  /**< The element is fully functional
                                              and is operating within normal
                                              operational parameters and
                                              without error. */
        LEP_HS_DEGRADED_WARNING      = 10, /**< The element is in working order
                                              and all functionality is provided.
                                              However, the element is not
                                              working to the best of its
                                              abilities. */
        LEP_HS_MINOR_FAILURE         = 15, /**< All functionality is available
                                              but some might be degraded. */
        LEP_HS_MAJOR_FAILURE         = 20, /**< The element is failing. It is
                                              possible that some or all of the
                                              functionality of this component is
                                              degraded or not working. */
        LEP_HS_CRITICAL_FAILURE      = 25, /**< The element is non-functional
                                              and recovery might not be
                                              possible. */
        LEP_HS_NON_RECOVERABLE_ERROR = 30  /**< The element has completely
                                              failed, and recovery is not
                                              possible. */
    };

    /**
     * CIM - OperatingStatus.
     */
    enum {
        LEP_OS_UNKNOWN,       /**< Indicates the implementation is in general
                                 capable of returning this property, but is
                                 unable to do so at this time. */
        LEP_OS_NOT_AVAILABLE, /**< Indicates that the implementation is capable
                                 of returning a value for this property, but not
                                 ever for this particular piece of
                                 hardware/software. */
        LEP_OS_SERVICING,     /**< Describes an element being configured,
                                 maintained, cleaned, or otherwise
                                 administered. */
        LEP_OS_STARTING,      /**< Describes an element being initialized. */
        LEP_OS_STOPPING,      /**< Describes an element being brought to an
                                 orderly stop. */
        LEP_OS_STOPPED,       /**< Clean and orderly stop. */
        LEP_OS_ABORTED,       /**< Abrupt stop. */
        LEP_OS_DORMANT,       /**< Indicates that the element is inactive or
                                 quiesced. */
        LEP_OS_COMPLETED,     /**< Indicates that the element has completed
                                 its operation. */
        LEP_OS_MIGRATING,     /**< Element is being moved between host
                                 elements. */
        LEP_OS_EMIGRATING,    /**< Element is being moved away from host
                                 element. */
        LEP_OS_IMMIGRATING,   /**< Element is being moved to new host
                                 element. */
        LEP_OS_SNAPSHOTTING,  /**< */
        LEP_OS_SHUTTING_DOWN, /**< Describes an element being brought to an
                                 abrupt stop. */
        LEP_OS_IN_TEST,       /**< Element is performing test functions. */
        LEP_OS_TRANSITIONING, /**< Describes an element that is between
                                 states. */
        LEP_OS_IN_SERVICE     /**< Describes an element that is in service
                                 and operational. */
    };

    /**
     * CIM - OperationalStatus.
     */
    enum {
        LEP_OPS_UNKNOWN,                    /**< Unknown operational status. */
        LEP_OPS_OTHER,                      /**< Other status. */
        LEP_OPS_OK,                         /**< The element is fully functional
                                               and is operating within normal
                                               operational. */
        LEP_OPS_DEGRADED,                   /**< The element is in working order
                                               and all functionality is
                                               provided. However, the element is
                                               not working to the best of its
                                               abilities. */
        LEP_OPS_STRESSED,                   /**< The element is functioning,
                                               but needs attention. */
        LEP_OPS_PREDICTIVE_FAILURE,         /**< Element is functioning
                                               nominally but predicting a
                                               failure in the near future. */
        LEP_OPS_ERROR,                      /**< In error state. */
        LEP_OPS_NON_RECOVERABLE_ERROR,      /**< Indicates that this element is
                                               in an error condition that
                                               requires human intervention. */
        LEP_OPS_STARTING,                   /**< Indicates that the element is
                                               in the process of going to an
                                               Enabled state. */
        LEP_OPS_STOPPING,                   /**< Describes an element being
                                               brought to an orderly stop. */
        LEP_OPS_STOPPED,                    /**< Clean and orderly stop. */
        LEP_OPS_IN_SERVICE,                 /**< Element being configured,
                                               maintained, cleaned, or otherwise
                                               administered. */
        LEP_OPS_NO_CONTACT,                 /**< The monitoring system has
                                               knowledge of this element, but
                                               has never been able to establish
                                               communications with it. */
        LEP_OPS_LOST_COMMUNICATION,         /**< The ManagedSystem Element is
                                               known to exist and has been
                                               contacted successfully in the
                                               past, but is currently
                                               unreachable. */
        LEP_OPS_ABORTED,                    /**< Abrupt stop. */
        LEP_OPS_DORMANT,                    /**< The element is inactive or
                                               quiesced. */
        LEP_OPS_SUPPORTING_ENTITY_IN_ERROR, /**< This element might be "OK" but
                                               that another element, on which it
                                               is dependent, is in error. */
        LEP_OPS_COMPLETED,                  /**< Tthe element has completed its
                                               operation. */
        LEP_OPS_POWER_MODE                  /**< The element has additional
                                               power model information. */
    };

    /**
     * CIM - PrimaryStatus.
     */
    enum {
        LEP_PS_UNKNOWN,  /**< Unknown primary status. */
        LEP_PS_OK,       /**< The element is fully functional and is operating
                            within normal operational. */
        LEP_PS_DEGRADED, /**< The element is in working order and all
                            functionality is provided. However, the element is
                            not working to the best of its abilities. */
        LEP_PS_ERROR     /**< In error state. */
    };

    /**
     * This structure stores the LANEndpoint's properties.
     * @todo How can obtain installation date form a device?
     *       char installDate[64];
     * @todo How can obtain device last change.
     *       char timeOfLastStateChange[64];
     */
    struct LANEndpoint {
        char aliasAddresses[32];             /**< Other unicast addresses that
                                                may be used to communicate with
                                                the LANEndpoint. */
        unsigned short availReqStates[9];    /**< The possible values for the
                                                RequestedState parameter of the
                                                method RequestStateChange. */
        char caption[64];                    /**< Short textual description of
                                                the object. */
        unsigned short communicationStatus;  /**< The ability of the
                                                instrumentation to communicate
                                                with the underlying
                                                ManagedElement. */
        char creationClassName[256];         /**< The name of the class or the
                                                subclass used in the creation of
                                                an instance. */
        char description[256];               /**< Textual description of the
                                                object. */
        unsigned short detailedStatus;       /**< Compliments PrimaryStatus with
                                                additional status detail. */
        char elementName[64];                /**< A user-friendly name for the
                                                object. */
        unsigned short enabledDefault;       /**< Administrator's default
                                                configuration for the Enabled
                                                State. */
        unsigned short enabledState;         /**< Indicates the enabled and
                                                disabled states of an
                                                element. */
        char groupAddresses[32];             /**< Multicast addresses to which
                                                the LANEndpoint listens. */
        unsigned short healthState;          /**< Current health of the
                                                element. */
        char instanceID[64];                 /**< Opaquely and uniquely identify
                                                an instance of this class. */
        char lanID[64];                      /**< Identifier for the LAN Segment
                                                to which the Endpoint is
                                                connected. */
        char macAddress[32];                 /**< The principal unicast address
                                                used in communication with the
                                                LANEndpoint. */
        unsigned int maxDataSize;            /**< The largest information field
                                                that may be sent or received by
                                                the LANEndpoint. */
        char name[256];                      /**< Identifies this
                                                ProtocolEndpoint. */
        char nameFormat[256];                /**< The naming heuristic that is
                                                selected to ensure that the
                                                value of the Name property is
                                                unique. */
        unsigned short operatingStatus;      /**< Current status value for the
                                                operational condition of the
                                                element. */
        unsigned short operationalStatus;    /**< Current statuses of the
                                                element. */
        char otherEnabledState[64];          /**< A string that describes the
                                                enabled or disabled state of
                                                the element. */
        char otherTypeDescription[64];       /**< Type of ProtocolEndpoint. */
        unsigned short primaryStatus;        /**< High level status value. */
        unsigned short protocolIFType;       /**< IANA ifType MIB. */
        unsigned short requestedState;       /**< The last requested or desired
                                                state for the element. */
        char statusDescriptions[256];        /**< Strings describing the various
                                                OperationalStatus array
                                                values. */
        char systemCreationClassName[256];   /**< The CreationClassName of the
                                                scoping System. */
        char systemName[256];                /**< The Name of the scoping
                                                System. */
        unsigned short transitioningToState; /**< The target state to which the
                                                instance is transitioning. */
    };

    /**
     * This structure is used as LANEndpoints list.
     */
    struct LANEndpointList {
        struct LANEndpoint     *sptr; /**< Pointer to current position
                                         in the list. */
        struct LANEndpointList *next; /**< Pointer to next position
                                         in the list. */
    };

    /**
     * This functions is used to get a list of LANEndpoints
     * based on nlLinkInfo filter.
     * @param lanEPList [out] LANEndpoint struct to be filled.
     * @param nlLinkInfo [in] link filter info.
     * @return LEP_OK=succesful | LEP_FAIL=fail
     */
    int getLANEndpoints(struct LANEndpointList **lanEPList,
                        const struct nlLinkInfo *nlLinkInfo);

    /**
     * This functions is used to get whole LANEndpoints
     * available in the system.
     * @param lanEPList [out] LANEndpoint struct to be filled.
     * @return LEP_OK=succesful | LEP_FAIL=fail
     */
    int getALLLANEndpoint(struct LANEndpointList **lanEPList);

    /**
     * This functions is used to get a route base on specified InstanceID.
     * @param lanEPList [out] LANEndpoint struct to be filled.
     * @param linkName [in] link ifname.
     * @return LEP_OK=succesful | LEP_FAIL=fail
     */
    int getLANEndpoint(struct LANEndpointList **lanEPList,
                       const char *linkName);

    /**
     * Used to set device status.
     * @param linkName [in] link name.
     * @param enabledState [in] desired state for the device.
     * @param timeoutPeriod [in] maximum amount of time that the
     * client expects the transition to the new state to take.
     * @return 0 = Completed with No Error
     *         1 = Not Supported
     *         2 = Unknown or Unspecified Error
     *         3 = Cannot complete within Timeout Period
     *         4 = Failed
     *         5 = Invalid Parameter
     *         6 = In Use
     *         7..4095 = DMTF Reserved
     *         4096 = Method Parameters Checked - Job Started
     *         4097 = Invalid State Transition
     *         4098 = Use of Timeout Parameter Not Supported
     *         4099 = Busy
     *         4100..32767 = Method Reserved
     *         32768..65535 = Vendor Specific
     */
    unsigned short changeLinkOPState(const char *linkName,
                                     unsigned short enabledState,
                                     unsigned long timeoutPeriod);

    /**
     * Converts a nlLinkInfoList structure to LANEndpointList strucutre.
     * @param nlLinkInfoList [in] list to be converted.
     * @param lanEPList [out] converted list.
     * @return LEP_OK=succesful | LEP_FAIL=fail
     */
    int nlListTOLanEPList(struct nlLinkInfoList **nlLinkInfoList,
                          struct LANEndpointList **lanEPList);

    /**
     * Converts a nlLinkInfo structure to LANEndpoint structure.
     * @param nlLinkInfo [in] structure to be converted.
     * @param lanEP [out] converted structure.
     * @return LEP_OK=succesful | LEP_FAIL=fail
     */
    int nlInfoTOLanEP(struct nlLinkInfo *nlLinkInfo,
                      struct LANEndpoint *lanEP);

    /**
     * This function is used to clean a LANEndpoint list.
     * @param lptr [in] list to be cleaned.
     * @return LEP_OK=succesful | LEP_FAIL=fail
     */
    int freeLANEndpointList(struct LANEndpointList *lptr);

    ////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif //_OSBASE_LANENDPOINT_H_
