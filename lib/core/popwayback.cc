/**
 *
 * Copyright (c) 2005-2012 POP-C++ project - GRID & Cloud Computing group, University of Applied Sciences of western
 *Switzerland.
 * http://gridgroup.hefr.ch/popc
 *
 * @author Valentin Clement (clementval)
 * @date 2010/04/19
 * @brief Implementation of the POPWayback object.
 *
 *
 */

/*
  Need refactoring to be in accordance:
    POPC_Wayback instead of POPWayback
 */

#include "popwayback.h"

#include <string.h>

/**
 * ViSaG : clementval
 * Constructor of the POPWayback class
 */
POPWayback::POPWayback() {
}

/**
 * ViSaG : clementval
 * Destructor of the POPWayback class
 */
POPWayback::~POPWayback() {
}

/**
 * ViSaG : clementval
 * Redefined the = operator for the object POPWayback
 * @param rhs Right hand-side parameter
 * @return The new POPWayback copied from the rhs
 */
const POPWayback& POPWayback::operator=(const POPWayback& rhs) {
    if (!rhs._lst_wb.empty()) {
        _lst_wb.clear();
        _lst_wb = rhs._lst_wb;
    }
    return (*this);
}

/**
 * ViSaG : clementval
 * Return the POPWayback object as a fromatted string
 * @return The formatted string corresponding to this object
 */
std::string POPWayback::getAsString() const {
    std::list<std::string> tmp = _lst_wb;
    std::string wb;
    while (!tmp.empty()) {
        std::string e = tmp.front();
        tmp.pop_front();
        wb.append(e);
        wb.append("//");
    }
    std::string wbstr = wb.c_str();
    return wbstr;
}

/**
 * ViSaG : clementval
 * Insert a node at the end of the way back. Do not insert it if the last node in the list is the same as the node given
 * in
 * parameter.
 * @param nodeId  The node ID to insert in the POPWayback
 */
void POPWayback::insertNode(std::string nodeId) {
    if (_lst_wb.empty()) {
        _lst_wb.push_back(nodeId);
    } else {
        std::string tmp = getNextNode();
        if (tmp == nodeId) {
            _lst_wb.push_back(nodeId);
        }
    }
}

/**
 * ViSaG : clementval
 * return the last Node
 * @return The node ID of the next node in the way back
 */
std::string POPWayback::getNextNode() const {
    std::string node = _lst_wb.back();
    return node;
}

/**
 * ViSaG : clementval
 * Delete the next node in the way back
 */
void POPWayback::deleteNextNode() {
    _lst_wb.pop_back();
}

/**
 * ViSaG : clementval
 * Check if it's the last node in the way back or if there is no node
 * @return Return true if there is 1 or 0 node in the way back
 */
bool POPWayback::isLastNode() const {
    if (_lst_wb.size() == 1 || _lst_wb.empty()) {
        return true;
    }
    return false;
}

/**
 * ViSaG : clementval
 * Check if the way back is empty
 * @return Return true if the way back is empty
 */
bool POPWayback::isEmpty() const {
    return _lst_wb.empty();
}

/**
 * ViSaG : clementval
 * Marshall or unmarshall the object to be sent over the network.
 * Remark : This method is overwritten from the POPBase class
 * @param buf The POPBuffer to be used for the (un)marshalling
 * @param If true=masrhalling, if false=unmarshalling
 */
void POPWayback::Serialize(POPBuffer& buf, bool pack) {
    if (pack) {
        // Pack the size
        int size = _lst_wb.size();
        buf.Pack(&size, 1);
        // Pack elements
        while (!_lst_wb.empty()) {
            std::string elt = _lst_wb.front();
            _lst_wb.pop_front();
            buf.Pack(&elt, 1);
        }
    } else {
        // Clear the current list
        _lst_wb.clear();
        // Unpack the size
        int size;
        buf.UnPack(&size, 1);
        // Unpack elements
        for (int i = 0; i < size; i++) {
            std::string elt;
            buf.UnPack(&elt, 1);
            _lst_wb.push_back(elt);
        }
    }
}
