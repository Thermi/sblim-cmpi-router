/**
 * @file
 * OSBase_NextHopIPRoute.c
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
 * Linux_NextHopIPRoute.
 * It is independent from any specific CIM technology.
 */

#include "OSBase_Common.h"
#include "OSBase_NextHopIPRoute.h"

#include <linux/rtnetlink.h>
#include <errno.h>

////////////////////////////////////////////////////////////////////////////////
// COMMON
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int getIPRoutes(struct nextHopIPList **nHopIPList,
                struct nlRouteInfo *nlRtInfo)
{
    _OSBASE_TRACE(3, ("--- getIPRoutes() called"));

    struct nlRouteInfoList *list = NULL;  // Netlink routes list.
    int                     rc   = NH_OK; // Result handler.

    // Check route info (used as filter).
    if(nlRtInfo == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid filter."));
        rc = NH_FAIL;
        goto out;
    }

    // Open netlink socket.
    if(nlOpenSocket() != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Unable to open netlink socket."));
        rc = NH_FAIL;
        goto out;
    }

    // Get routes.
    if(nlGetRoutes(&list, nlRtInfo) != NL_OK) {
        nlCloseSocket();
        _OSBASE_TRACE(1, ("[ERROR] - Unable to get NextHopIPRoutes."));
        rc = NH_FAIL;
        goto out;
    }

    // Close netlink socket.
    if(nlCloseSocket() != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Unable to close netlink socket."));
        rc = NH_FAIL;
        goto out;
    }

    // Convert list.
    if(nlListTOnhList(&list, nHopIPList) != NH_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not convert netlink routes list " \
                          "to NextHopIPRoute routes list."));
        rc = NH_FAIL;
        goto out;
    }

 out:
    nlFreeRouteInfoList(list);
    if(rc == NH_FAIL) {
        freeNextHopIPList(*nHopIPList);
        FREE_SAFE(nHopIPList);
    }

    _OSBASE_TRACE(3, ("--- getIPRoutes() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int getAllIPRoutes(struct nextHopIPList **nHopIPList)
{
    _OSBASE_TRACE(3, ("--- getAllIPRoutes() called"));

    struct nlRouteInfo nlRtInfo;   // Filter.
    int                rc = NH_OK; // Result handler.

    // Generic route info (used as filter).
    if(nlCreateDefaultRtInfo(&nlRtInfo) != NH_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not create default route filter."));
        rc = NH_FAIL;
        goto out;
    }

    // Get IPv4 routes.
    if(getIPRoutes(nHopIPList, &nlRtInfo) != NH_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not get IPv4 routes."));
        rc = NH_FAIL;
        goto out;
    }

    // Get IPv6 routes.
    nlRtInfo.family = AF_INET6;
    if(getIPRoutes(nHopIPList, &nlRtInfo) != NH_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not get IPv6 routes."));
        rc = NH_FAIL;
        goto out;
    }

 out:
    _OSBASE_TRACE(3, ("--- getAllIPRoutes() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int getIPRouteIId(struct nextHopIPList **nHopIPList, const char *instanceId)
{
    _OSBASE_TRACE(3, ("--- getIPRouteIId() called"));

    char               hostName[MAXHOSTNAMELEN]; // HostName of the system.
    struct nlRouteInfo nlRtInfo;                 // Filter.
    int                rc = NH_OK;               // Result handler.

    // Generic route info.
    if(nlCreateDefaultRtInfo(&nlRtInfo) != NH_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not create default route filter."));
        rc = NH_FAIL;
        goto out;
    }

    // Parse InstanceId.
    sscanf(instanceId, INSTANCEID_FORMAT_PARSE,
           hostName, nlRtInfo.dstAddr, &(nlRtInfo.dstLen),
           &(nlRtInfo.table), &(nlRtInfo.outputIf), &(nlRtInfo.scope),
           nlRtInfo.gw);
    if(getIPRoutes(nHopIPList, &nlRtInfo) != NH_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not get IP route."));
        rc = NH_FAIL;
        goto out;
    }

 out:
    _OSBASE_TRACE(3, ("--- getIPRouteIId() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int addIPRoute(struct nextHopIP *nHopIP)
{
    _OSBASE_TRACE(3, ("--- addIPRoute() called"));

    struct nlRouteInfo nlRtInfo;   // Filter.
    int                rc = NH_OK; // Result handler.

    // Generic route info.
    if(nlCreateDefaultRtInfo(&nlRtInfo) != NH_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not create default route filter."));
        rc = NH_FAIL;
        goto out;
    }

    // Convert info.
    if(nhTOnlInfo(nHopIP, &nlRtInfo) != NH_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not convert route to be added to " \
                          "netlink structure."));
        rc = NH_FAIL;
        goto out;
    }

    // Open netlink socket.
    if(nlOpenSocket() != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Unable to open netlink socket."));
        rc = NH_FAIL;
        goto out;
    }

    // Add route.
    if(nlModifyRoute(&nlRtInfo, RTM_NEWROUTE,
                     (NLM_F_CREATE | NLM_F_EXCL)) != NL_OK) {
        nlCloseSocket();
        _OSBASE_TRACE(1, ("[ERROR] - Could not add desired route."));
        rc = NH_FAIL;
        goto out;
    }

    // Close netlink socket.
    if(nlCloseSocket() != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Unable to close netlink socket."));
        rc = NH_FAIL;
        goto out;
    }

 out:
    _OSBASE_TRACE(3, ("--- addIPRoute() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int delIPRoute(const struct nextHopIP *nHopIP)
{
    _OSBASE_TRACE(3, ("--- delIPRoute() called"));

    char               hostName[MAXHOSTNAMELEN];   // System hostname.
    struct nlRouteInfo nlRtInfo;                   // Filter.
    int                rc = NH_OK;                 // Result handler.

    // Generic route info.
    if(nlCreateDefaultRtInfo(&nlRtInfo) != NH_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not create default route filter."));
        rc = NH_FAIL;
        goto out;
    }

    // Parse InstanceId.
    sscanf(nHopIP->instanceID, INSTANCEID_FORMAT_PARSE,
           hostName, nlRtInfo.dstAddr, &(nlRtInfo.dstLen),
           &(nlRtInfo.table), &(nlRtInfo.outputIf), &(nlRtInfo.scope),
           nlRtInfo.gw);

    // Open netlink socket.
    if(nlOpenSocket() != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Unable to open netlink socket."));
        rc = NH_FAIL;
        goto out;
    }

    // Del route.
    if(nlModifyRoute(&nlRtInfo, RTM_DELROUTE, 0) != NL_OK) {
        nlCloseSocket();
        _OSBASE_TRACE(1, ("[ERROR] - Could not remove desired route."));
        rc = NH_FAIL;
        goto out;
    }

    // Close netlink socket.
    if(nlCloseSocket() != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Unable to close netlink socket."));
        rc = NH_FAIL;
        goto out;
    }

 out:
    _OSBASE_TRACE(3, ("--- delIPRoute() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlListTOnhList(struct nlRouteInfoList **nlRtInfoList,
                   struct nextHopIPList **nHopIPList)
{
    _OSBASE_TRACE(3, ("--- nlListTOnhList() called"));

    struct nextHopIPList   *nHopIPListCurr   = NULL;  // Current next-hop route.
    struct nlRouteInfoList *nlRtInfoListCurr = NULL;  // Current netlink route.
    int                     rc               = NH_OK; // Result handler.

    nlRtInfoListCurr = *nlRtInfoList;
    while(nlRtInfoListCurr != NULL) {
        if(nlRtInfoListCurr->sptr != NULL) {
            // Create new route struct and temp containers variables.
            nHopIPListCurr = malloc(sizeof(*nHopIPListCurr));
            if(nHopIPListCurr == NULL) {
                _OSBASE_TRACE(1, ("[ERROR] - Could not alloc memory for " \
                                  "\'nHopIPListCurr\'."));
                rc = NH_FAIL;
                goto out;
            }
            memset(nHopIPListCurr, 0, sizeof(*nHopIPListCurr));
            
            nHopIPListCurr->sptr = malloc(sizeof(*(nHopIPListCurr->sptr)));
            if(nHopIPListCurr->sptr == NULL) {
                FREE_SAFE(nHopIPListCurr);
                _OSBASE_TRACE(1, ("[ERROR] - Could not alloc memory for " \
                                  "\'nHopIPListCurr->sptr\'."));
                rc = NH_FAIL;
                goto out;
            }
            memset(nHopIPListCurr->sptr, 0, sizeof(*(nHopIPListCurr->sptr)));

            // Convert from nl to nh.
            if(nlInfoTOnh(nlRtInfoListCurr->sptr,
                          nHopIPListCurr->sptr) != NH_OK) {
                freeNextHopIPList(nHopIPListCurr);
                _OSBASE_TRACE(1, ("[ERROR] - Could not convert structure " \
                                  "from nl to nh."));
                rc = NH_FAIL;
                goto out;
            }

            // Set next Next-hop.
            nHopIPListCurr->next = *nHopIPList;
            *nHopIPList = nHopIPListCurr;
        }
        nlRtInfoListCurr = nlRtInfoListCurr->next;
    }

 out:
    _OSBASE_TRACE(3, ("--- nlListTOnhList() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nhTOnlInfo(struct nextHopIP *nHopIP, struct nlRouteInfo *nlRtInfo)
{
    _OSBASE_TRACE(3, ("--- nhTOnlInfo() called"));

    int rc = NH_OK; // Result handler.

    // family.
    if(nHopIP->addressType == NH_AT_UNKNOWN) {
        nHopIP->addressType = getAddrType(nHopIP->dstAddress);
    }
    if(nHopIP->addressType == NH_AT_IPV4) {
        nlRtInfo->family = AF_INET;
    } else if(nHopIP->addressType == NH_AT_IPV6) {
        nlRtInfo->family = AF_INET6;
    } else {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid family."));
        rc = NH_FAIL;
        goto out;
    }

    // type.
    if((nlRtInfo->type > NH_RTTY_XRESOLVE) ||
       (nlRtInfo->type < NH_RTTY_UNSPEC)) {
        nlRtInfo->type = NH_RTTY_UNSPEC;
    } else {
        nlRtInfo->type = (int)nHopIP->routeType;
    }

    // protocol.
    // @todo obtain from Quagga.
    if(nHopIP->routeDerivation > NH_RTD_USER_DEFINED) {
        nlRtInfo->protocol = RTPROT_ZEBRA;
    } else if(nHopIP->routeDerivation == NH_RTD_USER_DEFINED) {
        nlRtInfo->protocol = RTPROT_STATIC;
    } else {
        nlRtInfo->protocol = RTPROT_UNSPEC;
    }

    // scope.
    if((nlRtInfo->scope > NH_RTS_NOWHERE) ||
       (nlRtInfo->scope < NH_RTS_UNIVERSE)) {
        nlRtInfo->scope = NH_RTS_UNIVERSE;
    } else {
        nlRtInfo->scope = (int)nHopIP->routeScope;
    }

    // srcLen.
    nlRtInfo->srcLen = 0;

    // dstLen.
    // @todo replace atoi for converter.
    if(nlRtInfo->family == AF_INET) {
        nlRtInfo->dstLen = atoi(nHopIP->dstMask);
    } else if(nlRtInfo->family = AF_INET6) {
        nlRtInfo->dstLen = nHopIP->prefixLength;
    }

    // tos.
    nlRtInfo->tos = 0;

    // dstAddr.
    if(nHopIP->dstAddress == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid dstAddress (mandatory)."));
        rc = NH_FAIL;
        goto out;
    } else {
        snprintf(nlRtInfo->dstAddr, 64, "%s", nHopIP->dstAddress);
    }

    // srcAddr.
    snprintf(nlRtInfo->srcAddr, 64, "%s", "");

    // inputIf.
    nlRtInfo->inputIf = 0;

    // outputIf.
    nlRtInfo->outputIf = (int)nHopIP->routeOutputIf;

    // gw.
    if(nHopIP->dstAddress == NULL) {
        snprintf(nlRtInfo->gw, 64, "%s", "");
    } else {
        snprintf(nlRtInfo->gw, 64, "%s", nHopIP->routeGateway);
    }

    // priority.
    nlRtInfo->priority = nHopIP->routeMetric;

    // prefSrc.
    snprintf(nlRtInfo->prefSrc, 64, "%s", "");

    // metrics.
    nlRtInfo->metrics = 0;

    // table.
    if((nlRtInfo->table > NH_RTT_LOCAL) ||
       (nlRtInfo->table < NH_RTT_UNSPEC)) {
        nlRtInfo->table = NH_RTTY_UNSPEC;
    } else {
        nlRtInfo->table = (int)nHopIP->routeTable;
    }

 out:
    _OSBASE_TRACE(3, ("--- nhTOnlInfo() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlInfoTOnh(const struct nlRouteInfo *nlRtInfo, struct nextHopIP *nHopIP)
{
    _OSBASE_TRACE(3, ("--- nlInfoTOnh() called"));

    char addrTypeStr[ADDR_TYPE_STR_MAXLEN];  // Temporary string.
    char tmpStr[256];                        // Used to store strings
                                             // temporarily.
    int  rc = NH_OK;                         // Result handler.

    // addressType.
    if(nlRtInfo->family == AF_INET) {
        nHopIP->addressType = NH_AT_IPV4;
    } else if(nlRtInfo->family == AF_INET6) {
        nHopIP->addressType = NH_AT_IPV6;
    } else {
        nHopIP->addressType = NH_AT_UNKNOWN;
    }
    if(getAddrTypeStr(nHopIP->addressType, addrTypeStr) != NH_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not get address type as string."));
        rc = NH_FAIL;
        goto out;
    }

    // dstAddress.
    snprintf(nHopIP->dstAddress, 64, "%s", nlRtInfo->dstAddr);

    // dstMask.
    if(nHopIP->addressType == NH_AT_IPV4) {
        snprintf(nHopIP->dstMask, 64, "%d", nlRtInfo->dstLen);
    } else {
        // Not used in IPv6.
        snprintf(nHopIP->dstMask, 64, "");
    }

    // prefixLength.
    if(nHopIP->addressType == NH_AT_IPV6) {
        nHopIP->prefixLength = nlRtInfo->dstLen;
    } else {
        // Not used in IPv4.
        nHopIP->prefixLength = 0;
    }

    // isStatic.
    if(nlRtInfo->protocol > RTPROT_STATIC) {
        // false.
        nHopIP->isStatic = 0;
    } else {
        // true.
        nHopIP->isStatic = 1;
    }

    // otherDerivation.
    snprintf(nHopIP->otherDerivation, 64, "");

    // routeDerivation.
    // @todo obtain from Quagga.
    if((nlRtInfo->protocol > RTPROT_STATIC) ||
       ((nlRtInfo->protocol > RTPROT_UNSPEC) &&
        (nlRtInfo->protocol < RTPROT_STATIC))) {
        nHopIP->routeDerivation = NH_RTD_OTHER;
    } else if(nlRtInfo->protocol == RTPROT_STATIC) {
        nHopIP->routeDerivation = NH_RTD_USER_DEFINED;
    } else {
        nHopIP->routeDerivation = NH_RTD_UNKNOWN;
    }

    // adminDistance.
    // @todo obtain from AdministrativeDistance and nHopIP->routeDerivation.
    nHopIP->adminDistance = 1;

    // routeMetric.
    nHopIP->routeMetric = nlRtInfo->priority;

    // typeOfRoute.
    if(nlRtInfo->protocol == RTPROT_STATIC) {
        nHopIP->typeOfRoute = NH_TOR_ADMINISTRATOR;
    } else {
        nHopIP->typeOfRoute = NH_TOR_COMPUTED;
    }

    // routeGateway.
    snprintf(nHopIP->routeGateway, 64, "%s", nlRtInfo->gw);

    // routeOutputIf.
    nHopIP->routeOutputIf = nlRtInfo->outputIf;

    // routeScope.
    nHopIP->routeScope = nlRtInfo->scope;

    // routeTable.
    nHopIP->routeTable = nlRtInfo->table;

    // routeType.
    nHopIP->routeType = nlRtInfo->type;

    // caption.
    snprintf(nHopIP->caption, 64, "NextHop %s route.", addrTypeStr);

    // description.
    if_indextoname(nHopIP->routeOutputIf, tmpStr);
    if(tmpStr == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Cannot get ifname. %s",
                          strerror(errno)));
        rc = NH_FAIL;
        goto out;
    }
    if(nHopIP->addressType == NH_AT_IPV4) {
        snprintf(nHopIP->description, 256,
                 "NextHop to %s/%s through \"%s\" interface.",
                 nHopIP->dstAddress, nHopIP->dstMask, tmpStr);
    } else if(nHopIP->addressType == NH_AT_IPV6) {
        snprintf(nHopIP->description, 256,
                 "NextHop to %s/%d through \"%s\" interface.",
                 nHopIP->dstAddress, nHopIP->prefixLength, tmpStr);
    } else {
        snprintf(nHopIP->description, 256,
                 "NextHop through \"%s\" interface.", tmpStr);
    }

    // elementName.
    if(nHopIP->addressType == NH_AT_IPV4) {
        snprintf(nHopIP->elementName, 64, "%s-%s/%s",
                 addrTypeStr, nHopIP->dstAddress, nHopIP->dstMask);
    } else if(nHopIP->addressType == NH_AT_IPV6) {
        snprintf(nHopIP->elementName, 64, "%s-%s/%d",
                 addrTypeStr, nHopIP->dstAddress, nHopIP->prefixLength);
    } else {
        snprintf(nHopIP->elementName, 64, "%s", addrTypeStr);
    }

    // instanceID.
    if(gethostname(tmpStr, MAXHOSTNAMELEN) != 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Cannot get hostname."));
        rc = NH_FAIL;
        goto out;
    }
    if(nHopIP->addressType == NH_AT_IPV6) {
        snprintf(nHopIP->instanceID, 64, INSTANCEID_FORMAT,
                 tmpStr, nHopIP->dstAddress, nHopIP->prefixLength,
                 nHopIP->routeTable, nHopIP->routeOutputIf, nHopIP->routeScope,
                 nHopIP->routeGateway);
    } else {
        snprintf(nHopIP->instanceID, 64, INSTANCEID_FORMAT,
                 tmpStr, nHopIP->dstAddress, atoi(nHopIP->dstMask),
                 nHopIP->routeTable, nHopIP->routeOutputIf, nHopIP->routeScope,
                 nHopIP->routeGateway);
    }

 out:
    _OSBASE_TRACE(3, ("--- nlInfoTOnh() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int getAddrType(const char *addr)
{
    _OSBASE_TRACE(3, ("--- getAddrType() called"));

    char tmp[64];            // Used for temporal storage.
    int  rc = NH_AT_UNKNOWN; // Result.

    if(inet_pton(AF_INET, addr, tmp) == 1) {
        // IPv4.
        rc = NH_AT_IPV4;
    } else if(inet_pton(AF_INET6, addr, tmp) == 1) {
        // IPv6.
        rc = NH_AT_IPV6;
    }

    _OSBASE_TRACE(3, ("--- getAddrType() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int getAddrTypeStr(int addressType, char *addrTypeStr)
{
    _OSBASE_TRACE(3, ("--- getAddrTypeStr() called"));

    int rc = NH_OK; // Result handler.

    if(addressType == NH_AT_IPV4) {
        snprintf(addrTypeStr, ADDR_TYPE_STR_MAXLEN, "%s", "IPv4");
    } else if(addressType == NH_AT_IPV6) {
        snprintf(addrTypeStr, ADDR_TYPE_STR_MAXLEN, "%s", "IPv6");
    } else {
        snprintf(addrTypeStr, ADDR_TYPE_STR_MAXLEN, "%s", "Unknown");
    }

    _OSBASE_TRACE(3, ("--- getAddrTypeStr() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int freeNextHopIPList(struct nextHopIPList *lptr)
{
    _OSBASE_TRACE(3, ("--- freeNextHopIPList() called"));

    struct nextHopIPList *ls = NULL;  // Temporary list container.
    int                   rc = NH_OK; // Result handler.

    // Check received parameters.
    if(lptr == NULL) {
        _OSBASE_TRACE(2, ("[INFO] - NULL nextHopIPList, nothing to free."));
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
    _OSBASE_TRACE(3, ("--- freeNextHopIPList() exited"));
    return rc;
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
