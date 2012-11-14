/**
 * @file
 * OSBase_LANEndpoint.c
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
 * This shared library provides resource access functionality for the class
 * Linux_LANEndpoint.
 * It is independent from any specific CIM technology.
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/rtnetlink.h>
#include <signal.h>
#include <errno.h>

#include "OSBase_Common.h"
#include "OSBase_LANEndpoint.h"


////////////////////////////////////////////////////////////////////////////////
// COMMON
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
/**
 * This function converts from datetime string interval to total number
 * of milliseconds (10e-3).
 * @param pDatetimeString [in] datetime string interval to be converted.
 * @todo move this function to the proper file (could be in common.h).
 * @return converted datetime string interval.
 */
unsigned long datetime_str_interval_to_ms(const char *pDatetimeString) {
    _OSBASE_TRACE(3, ("--- datetime_str_interval_to_ms() called"));

    int days = 0, hours = 0, minutes = 0, seconds = 0, mseconds = 0;
    unsigned long totalSeconds = 0;

    sscanf(pDatetimeString, "%8d%2d%2d%2d.%6d:000", &days, &hours,
           &minutes, &seconds, &mseconds);
    totalSeconds += (mseconds);
    totalSeconds += (seconds*1000);
    totalSeconds += (minutes*1000*60);
    totalSeconds += (hours*1000*60*60);
    totalSeconds += (days*1000*60*60*24);

    _OSBASE_TRACE(3, ("--- datetime_str_interval_to_ms() exited"));
    return totalSeconds;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int getLANEndpoints(struct LANEndpointList **lanEPList,
                    const struct nlLinkInfo *nlLinkInfo)
{
    _OSBASE_TRACE(3, ("--- getLANEndpoints() called"));

    struct nlLinkInfoList *list = NULL;   // Netlink links list.
    int                    rc   = LEP_OK; // Result handler.

    // Check link info (used as filter).
    if(nlLinkInfo == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid filter."));
        rc = LEP_FAIL;
        goto out;
    }

    // Open netlink socket.
    if(nlOpenSocket() != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Unable to open netlink socket."));
        rc = LEP_FAIL;
        goto out;
    }

    // Get LANEndpoint.
    if(nlGetLinks(&list, nlLinkInfo) != NL_OK) {
        nlCloseSocket();
        _OSBASE_TRACE(1, ("[ERROR] - Unable to get LANEndpoint."));
        rc = LEP_FAIL;
        goto out;
    }

    // Close netlink socket.
    if(nlCloseSocket() != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Unable to close netlink socket."));
        rc = LEP_FAIL;
        goto out;
    }

    // Convert list.
    if(nlListTOLanEPList(&list, lanEPList) != LEP_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not convert netlink links list to " \
                          "LANEndpoint links list."));
        rc = LEP_FAIL;
        goto out;
    }

 out:
    nlFreeLinkInfoList(list);
    if(rc == LEP_FAIL) {
        freeLANEndpointList(*lanEPList);
        FREE_SAFE(lanEPList);
    }

    _OSBASE_TRACE(3, ("--- getLANEndpoints() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int getALLLANEndpoints(struct LANEndpointList **list)
{
    _OSBASE_TRACE(3, ("--- getALLLANEndpoints() called"));

    struct nlLinkInfo nlLinkInfo;  // Filter.
    int               rc = LEP_OK; // Result handler.

    // Generic link info (used as filter).
    if(nlCreateDefaultLinkInfo(&nlLinkInfo) != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not create default link " \
                          "info filter."));
        rc = LEP_FAIL;
        goto out;
    }

    // Get LANEndpoint.
    if(getLANEndpoints(list, &nlLinkInfo) != LEP_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not get LANEndpoints."));
        rc = LEP_FAIL;
        goto out;
    }

 out:
    _OSBASE_TRACE(3, ("--- getALLLANEndpoints() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int getLANEndpoint(struct LANEndpointList **lanEPList, const char *linkName)
{
    _OSBASE_TRACE(3, ("--- getLANEndpoint() called"));

    struct nlLinkInfo nlLinkInfo;  // Filter.
    int               rc = LEP_OK; // Result handler.

    // Generic link info (used as filter).
    if(nlCreateDefaultLinkInfo(&nlLinkInfo) != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not create default link " \
                          "info filter."));
        rc = LEP_FAIL;
        goto out;
    }
    
    // Add ifname.
    snprintf(nlLinkInfo.ifname, IFNAMSIZ, "%s", linkName);

    // Get LANEndpoint.
    if(getLANEndpoints(lanEPList, &nlLinkInfo) != LEP_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not get LANEndpoints."));
        rc = LEP_FAIL;
        goto out;
    }

 out:
    _OSBASE_TRACE(3, ("--- getLANEndpoint() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
unsigned short changeLinkOPState(const char *linkName,
                                 unsigned short enabledState,
                                 unsigned long timeoutPeriod)
{
    _OSBASE_TRACE(3, ("--- changeLinkOPState() called"));

    struct nlLinkInfo nlLinkInfo; // Filter.
    pid_t             cpid = 0;   // Child process id.
    unsigned short    rc   = 0;   // Result.

    // Creating a child process.
    if(timeoutPeriod != 0) {
        cpid = fork();
        if(cpid == -1) {
            rc = 2;
            goto out;
        }
    }

    if(cpid == 0) { // Code executed by child.
        // Generic link info.
        if(nlCreateDefaultLinkInfo(&nlLinkInfo) != NL_OK) {
            _OSBASE_TRACE(1, ("[ERROR] - Could not create default link info."));
            rc = 4;
            goto out;
        }

        switch(enabledState) {
        case LEP_RS_ENABLED:
            nlLinkInfo.change |= IFF_UP;
            nlLinkInfo.flags |= IFF_UP;
            // nlLinkInfo.operstate = IF_OPER_UP;
            break;
        case LEP_RS_DISABLED:
            nlLinkInfo.change |= IFF_UP;
            nlLinkInfo.flags &= ~IFF_UP;
            // nlLinkInfo.operstate = IF_OPER_DOWN;
            break;
        case LEP_RS_SHUT_DOWN:
        case LEP_RS_OFFLINE:
            // nlLinkInfo.operstate = IF_OPER_DORMANT;
            break;
        case LEP_RS_TEST:
            // nlLinkInfo.operstate = IF_OPER_TESTING;
            break;
        case LEP_RS_DEFERRED:
        case LEP_RS_QUIESCE:
        case LEP_RS_REBOOT:
        case LEP_RS_RESET:
            break;
        default:
            rc = 5;
            goto out;
        }

        // Setting link ifname.
        snprintf(nlLinkInfo.ifname, IFNAMSIZ, "%s", linkName);

        // Open netlink socket.
        if(nlOpenSocket() != NL_OK) {
            _OSBASE_TRACE(1, ("[ERROR] - Unable to open netlink socket."));
            rc = 4;
            goto out;
        }

        // Change state.
        if(nlModifyLink(&nlLinkInfo, RTM_NEWLINK, 0) != NL_OK) {
            nlCloseSocket();
            _OSBASE_TRACE(1, ("[ERROR] - Unable to change link operative " \
                              "state."));
            rc = 4;
            goto out;
        }

        // Close netlink socket.
        if(nlCloseSocket() != NL_OK) {
            _OSBASE_TRACE(1, ("[ERROR] - Unable to close netlink socket."));
            rc = 4;
            goto out;
        }
    } else { // Code executed by parent.
        // Kill child after specified milliseconds.
        if(usleep(timeoutPeriod) != 0) {
            _OSBASE_TRACE(1, ("[ERROR] - usleep error. %s", strerror(errno)));
            rc = 2;
            goto out;
        }
        if(kill(cpid, SIGKILL) != 0) {
            _OSBASE_TRACE(1, ("[ERROR] - Could not kill process. %s",
                              strerror(errno)));
            rc = 2;
            goto out;
        }
        cpid = -1;
        rc = 3;
    }

 out:
    _OSBASE_TRACE(3, ("--- changeLinkOPState() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlListTOLanEPList(struct nlLinkInfoList **nlLinkInfoList,
                      struct LANEndpointList **lanEPList)
{
    _OSBASE_TRACE(3, ("--- nlListTOLanEPList() called"));

    struct nlLinkInfoList  *nlLinkInfoListCurr = NULL;   // Current link info.
    struct LANEndpointList *lanEPListCurr      = NULL;   // Current LANEndpoint.
    int                     rc                 = LEP_OK; // Result handler.

    nlLinkInfoListCurr = *nlLinkInfoList;
    while(nlLinkInfoListCurr != NULL) {
        if(nlLinkInfoListCurr->sptr != NULL) {
            // Create new LANEndpoint struct and temp containers variables.
            lanEPListCurr = malloc(sizeof(*lanEPListCurr));
            if(lanEPListCurr == NULL) {
                _OSBASE_TRACE(1, ("[ERROR] - Could not alloc memory for " \
                                  "\'lanEPListCurr\'."));
                rc = LEP_FAIL;
                goto out;
            }
            memset(lanEPListCurr, 0, sizeof(*lanEPListCurr));

            lanEPListCurr->sptr = malloc(sizeof(*(lanEPListCurr->sptr)));
            if(lanEPListCurr->sptr == NULL) {
                FREE_SAFE(lanEPListCurr);
                _OSBASE_TRACE(1, ("[ERROR] - Could not alloc memory for " \
                                  "\'lanEPListCurr->sptr\'."));
                rc = LEP_FAIL;
                goto out;
            }
            memset(lanEPListCurr->sptr, 0, sizeof(*(lanEPListCurr->sptr)));

            // Convert from nl to LANEndpoint.
            if(nlInfoTOLanEP(nlLinkInfoListCurr->sptr,
                             lanEPListCurr->sptr) != LEP_OK) {
                freeLANEndpointList(lanEPListCurr);
                _OSBASE_TRACE(1, ("[ERROR] - Could not convert structure " \
                                  "from nl to LANEndpoint."));
                rc = LEP_FAIL;
                goto out;
            }

            // Set next LANEndpoint.
            lanEPListCurr->next = *lanEPList;
            *lanEPList = lanEPListCurr;
        }
        nlLinkInfoListCurr = nlLinkInfoListCurr->next;
    }

 out:
    _OSBASE_TRACE(3, ("--- nlListTOLanEPList() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlInfoTOLanEP(struct nlLinkInfo *nlLinkInfo,
                  struct LANEndpoint *lanEP)
{
    _OSBASE_TRACE(3, ("--- nlInfoTOLanEP() called"));

    int typePos = 0;      // Link type.
    int rc      = LEP_OK; // Result handler.

    // MacAddress.
    nlAddr_n2a(nlLinkInfo->address, nlLinkInfo->addressLen,
               nlLinkInfo->type, lanEP->macAddress, 32);

    // Name.
    snprintf(lanEP->name, 256, "%s", nlLinkInfo->ifname);

    // Caption.
    snprintf(lanEP->caption, 64, LANENDPOINT_CAPTION);

    // CreationClassName.
    snprintf(lanEP->creationClassName, 256, "%s", CREATION_CLASS_NAME);

    // Description.
    snprintf(lanEP->description, 256, "%s", LANENDPOINT_DESC);

    // ElementName.
    snprintf(lanEP->elementName, 64, "%s_%d", lanEP->name, nlLinkInfo->index);

    // CommunicationStatus, DetailedStatus, EnabledState, HealthState,
    // OperatingStatus, PrimaryStatus, OperationalStatus.
    if(nlLinkInfo->operstate == IF_OPER_UNKNOWN) {
        if((nlLinkInfo->flags & IFF_UP) &&
           (nlLinkInfo->flags & IFF_LOWER_UP)) {
            lanEP->communicationStatus = LEP_CS_COMMUNICATION_OK;
            lanEP->detailedStatus = LEP_DS_NO_ADDITIONAL_INFORMATION;
            lanEP->enabledState = LEP_ES_ENABLED;
            lanEP->healthState = LEP_HS_OK;
            lanEP->operatingStatus = LEP_OS_IN_SERVICE;
            lanEP->primaryStatus = LEP_PS_OK;
            lanEP->operationalStatus = LEP_OPS_OK;
        } else {
            lanEP->communicationStatus = LEP_CS_UNKNOWN;
            lanEP->detailedStatus = LEP_DS_NON_RECOVERABLE_ERROR;
            lanEP->enabledState = LEP_ES_DISABLED;
            lanEP->healthState = LEP_HS_UNKNOWN;
            lanEP->operatingStatus = LEP_OS_UNKNOWN;
            lanEP->primaryStatus = LEP_PS_UNKNOWN;
            lanEP->operationalStatus = LEP_OPS_UNKNOWN;
        }

    } else if(nlLinkInfo->operstate == IF_OPER_NOTPRESENT) {
        lanEP->communicationStatus = LEP_CS_UNKNOWN;
        lanEP->detailedStatus = LEP_DS_NON_RECOVERABLE_ERROR;
        lanEP->enabledState = LEP_ES_DISABLED;
        lanEP->healthState = LEP_HS_NON_RECOVERABLE_ERROR;
        lanEP->operatingStatus = LEP_OS_ABORTED;
        lanEP->primaryStatus = LEP_PS_ERROR;
        lanEP->operationalStatus = LEP_OPS_NON_RECOVERABLE_ERROR;

    } else if(nlLinkInfo->operstate == IF_OPER_DOWN) {
        lanEP->communicationStatus = LEP_CS_LOST_COMMUNICATION;
        lanEP->detailedStatus = LEP_DS_NON_RECOVERABLE_ERROR;
        lanEP->enabledState = LEP_ES_DISABLED;
        lanEP->healthState = LEP_HS_CRITICAL_FAILURE;
        lanEP->operatingStatus = LEP_OS_STOPPED;
        lanEP->primaryStatus = LEP_PS_ERROR;
        lanEP->operationalStatus = LEP_OPS_STOPPED;

    } else if(nlLinkInfo->operstate == IF_OPER_LOWERLAYERDOWN) {
        lanEP->communicationStatus = LEP_CS_LOST_COMMUNICATION;
        lanEP->detailedStatus = LEP_DS_SUPPORTING_ENTITY_IN_ERROR;
        lanEP->enabledState = LEP_ES_DISABLED;
        lanEP->healthState = LEP_HS_MAJOR_FAILURE;
        lanEP->operatingStatus = LEP_OS_ABORTED;
        lanEP->primaryStatus = LEP_PS_ERROR;
        lanEP->operationalStatus = LEP_OPS_SUPPORTING_ENTITY_IN_ERROR;

    } else if(nlLinkInfo->operstate == IF_OPER_TESTING) {
        lanEP->communicationStatus = LEP_CS_LOST_COMMUNICATION;
        lanEP->detailedStatus = LEP_DS_NON_RECOVERABLE_ERROR;
        lanEP->enabledState = LEP_ES_IN_TEST;
        lanEP->healthState = LEP_HS_MINOR_FAILURE;
        lanEP->operatingStatus = LEP_OS_IN_TEST;
        lanEP->primaryStatus = LEP_PS_DEGRADED;
        lanEP->operationalStatus = LEP_OPS_IN_SERVICE;

    } else if(nlLinkInfo->operstate == IF_OPER_DORMANT) {
        lanEP->communicationStatus = LEP_CS_LOST_COMMUNICATION;
        lanEP->detailedStatus = LEP_DS_STRESSED;
        lanEP->enabledState = LEP_ES_QUIESCE;
        lanEP->healthState = LEP_HS_CRITICAL_FAILURE;
        lanEP->operatingStatus = LEP_OS_DORMANT;
        lanEP->primaryStatus = LEP_PS_DEGRADED;
        lanEP->operationalStatus = LEP_OPS_DORMANT;

    } else if(nlLinkInfo->operstate == IF_OPER_UP) {
        lanEP->communicationStatus = LEP_CS_COMMUNICATION_OK;
        lanEP->detailedStatus = LEP_DS_NO_ADDITIONAL_INFORMATION;
        lanEP->enabledState = LEP_ES_ENABLED;
        lanEP->healthState = LEP_HS_OK;
        lanEP->operatingStatus = LEP_OS_IN_SERVICE;
        lanEP->primaryStatus = LEP_PS_OK;
        lanEP->operationalStatus = LEP_OPS_OK;

    } else {
        lanEP->communicationStatus = LEP_CS_LOST_COMMUNICATION;
        lanEP->detailedStatus = LEP_DS_NOT_AVAILABLE;
        lanEP->enabledState = LEP_ES_DISABLED;
        lanEP->healthState = LEP_HS_UNKNOWN;
        lanEP->operatingStatus = LEP_OS_UNKNOWN;
        lanEP->primaryStatus = LEP_PS_UNKNOWN;
        lanEP->operationalStatus = LEP_OPS_UNKNOWN;
    }

    // InstanceID.
    snprintf(lanEP->instanceID, 64, "%s-%s", lanEP->name, lanEP->macAddress);

    // LanID.
    snprintf(lanEP->lanID, 64, "NULL");

    // MaxDataSize.
    lanEP->maxDataSize = nlLinkInfo->mtu;

    // NameFormat.
    // We look the position of the link type into linkType array, then
    // we can obtain the string representation from linkTypeName.
    typePos = nlGetLinkTypePos(nlLinkInfo->type);
    snprintf(lanEP->nameFormat, 256, "%s", linkTypeName[typePos]);

    // OtherEnabledState.
    snprintf(lanEP->otherEnabledState, 64, "");

    // OtherTypeDescription.
    snprintf(lanEP->otherTypeDescription, 64, "");

    // ProtocolIFType.
    // @todo implement, we need to take this info from snmp.
    lanEP->protocolIFType = 0;

    // RequestedState.
    // @todo implement.
    lanEP->requestedState = LEP_RS_UNKNOWN;

    // SystemCreationClassName.
    // @todo Defined in cmpiOSBase_LANEndpoint.c, we should obtain it.
    snprintf(lanEP->systemCreationClassName, 256, "");

    // SystemName.
    gethostname(lanEP->systemName, MAXHOSTNAMELEN);

    // TransitioningToState.
    // @todo implement.
    lanEP->transitioningToState = LEP_RS_UNKNOWN;

    // InstallDate.
    // @todo implement. how can obtain installation date form a device?
    // snprintf(lanEP->installDate, 64, "%s", );

    // TimeOfLastStateChange.
    // @todo implement. How can obtain device last change?.
    // snprintf(lanEP->timeOfLastStateChange, 64, "%s", );

    // StatusDescriptions.
    snprintf(lanEP->statusDescriptions, 256, "");

    // GroupAddresses.
    // @todo implement (use CTRL_ATTR_MCAST_GROUPS).
    snprintf(lanEP->groupAddresses, 32, "");

    // AliasAddresses.
    // @todo implement. Is this supported on linux?
    snprintf(lanEP->aliasAddresses, 32, "");

    // AvailableRequestedStates.
    // @todo implement CIM_EnabledLogicalElementCapabilities.
    lanEP->availReqStates[0] = LEP_RS_ENABLED;
    lanEP->availReqStates[1] = LEP_RS_DISABLED;
    lanEP->availReqStates[2] = LEP_RS_SHUT_DOWN;
    lanEP->availReqStates[3] = LEP_RS_OFFLINE;
    lanEP->availReqStates[4] = LEP_RS_TEST;
    lanEP->availReqStates[5] = LEP_RS_DEFERRED;
    lanEP->availReqStates[6] = LEP_RS_QUIESCE;
    lanEP->availReqStates[7] = LEP_RS_REBOOT;
    lanEP->availReqStates[8] = LEP_RS_RESET;

 out:
    _OSBASE_TRACE(3, ("--- nlInfoTOLanEP() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int freeLANEndpointList(struct LANEndpointList *lptr)
{
    _OSBASE_TRACE(3,("--- freeLANEndpointList() called"));

    struct LANEndpointList *ls = NULL;   // Temporary list container.
    int                     rc = LEP_OK; // Result handler.

    // Check received parameters.
    if(lptr == NULL) {
        _OSBASE_TRACE(2, ("[INFO] - NULL LANEndpoints list, nothing to free."));
        goto out;
    }

    // Clean.
    while(lptr != NULL) {
        FREE_SAFE(lptr->sptr);
        ls = lptr;
        lptr = lptr->next;
        FREE_SAFE(ls);
    }

 out:
    _OSBASE_TRACE(3,("--- freeLANEndpointList() exited"));
    return rc;
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
