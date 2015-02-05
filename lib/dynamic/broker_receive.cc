/**
 *
 * Copyright (c) 2005-2012 POP-C++ project - GRID & Cloud Computing group, University of Applied Sciences of western Switzerland.
 * http://gridgroup.hefr.ch/popc
 *
 * @author Tuan Anh Nguyen
 * @date 2005/01/01
 * @brief Implementation of parallel object broker : receive requests.
 *
 *
 */

/*
  Deeply need refactoring:
    POPC_Broker instead of paroc_broker
 */

#include "popc_intface.h"

#include <iostream>
#include <fstream>

#include "paroc_broker.h"
#include "paroc_interface.h"
#include "paroc_event.h"
#include "paroc_buffer_factory.h"
#include "paroc_buffer_factory_finder.h"
#include "paroc_system.h"

bool NewConnection(void *dat, paroc_connection *conn) {
    paroc_broker *br=(paroc_broker *)dat;
    return br->OnNewConnection(conn);
}


bool CloseConnection(void *dat, paroc_connection *conn) {
    paroc_broker *br = (paroc_broker*)dat;
    return br->OnCloseConnection(conn);
}


/**
 * Receive request and put request in the FIFO
 */
void paroc_broker::ReceiveThread(paroc_combox *server) { // Receive request and put request in the FIFO
    server->SetCallback(COMBOX_NEW, NewConnection, this);
    server->SetCallback(COMBOX_CLOSE, CloseConnection, this);

    while(state==POPC_STATE_RUNNING) {
        paroc_request req;
        req.data=NULL;
        try {
            if(!ReceiveRequest(server, req)) {
                break;
            }


            // Is it a POP-C++ core call ? If so serve it right away
            if(ParocCall(req)) {
                if(req.data!=NULL) {
                    req.data->Destroy();
                }
                execCond.broadcast();
                continue;
            }
            // Register the request to be served by the broker serving thread
            RegisterRequest(req);
        } catch(...) {
            LOG_WARNING("Exception in paroc_broker::ReceiveThread");
            if(req.data != NULL) {
                req.data->Destroy();
            }
            execCond.broadcast();
            break;
        }
    }
    LOG_DEBUG("Exiting receive thread %s", paroc_broker::accesspoint.GetAccessString());
    server->Close();
}


bool paroc_broker::ReceiveRequest(paroc_combox *server, paroc_request &req) {
    server->SetTimeout(-1);
    while(1) {
        // Waiting for a new connection or a new request
        paroc_connection *conn = server->Wait();

        // Trouble with the connection
        if(conn == NULL) {
            execCond.broadcast();
            return false;
        }

        if(conn->is_initial_connection()) {
            continue;
        }

        // Receiving the real data
        paroc_buffer_factory *fact = conn->GetBufferFactory();
        req.data = fact->CreateBuffer();

        if(req.data->Recv(conn)) {
            req.from = conn;
            const paroc_message_header &h = req.data->GetHeader();
            req.methodId[0] = h.GetClassID();
            req.methodId[1] = h.GetMethodID();

            if(!((req.methodId[2]=h.GetSemantics()) & INVOKE_SYNC)) {
#ifdef OD_DISCONNECT
                if(checkConnection) {
                    server->SendAck(conn);
                }
#endif
            }
            return true;
        }

        req.data->Destroy();
    }
    return false;
}


void paroc_broker::RegisterRequest(paroc_request &req) {
    //Check if mutex is waiting/executing...
    int type=req.methodId[2];

    if(type & INVOKE_SYNC) {
        // Method call is synchronous, a response will be send back so the connection is saved
        req.from  = req.from->Clone();
    } else {
        // Method call is asynchronous so the connection is not needed anymore
        req.from = NULL;
    }


    if(type & INVOKE_CONC) {    // Method semantic is concurrent so trying to execute if there is no mutex pending
        mutexCond.lock();
        if(mutexCount<=0) {
            ServeRequest(req);
            mutexCond.unlock();
            return;
        }
    } else if(type & INVOKE_MUTEX) {  // Method semantic is mutex, adding one to the number of mutex pending
        mutexCond.lock();
        mutexCount++;
        mutexCond.unlock();
    }

    // Adding the request to the request queue
    execCond.lock();
    request_fifo.AddTail(req);
    int count=request_fifo.GetCount();

    execCond.broadcast();
    execCond.unlock();

    if(type & INVOKE_CONC) {
        concPendings++;
        mutexCond.unlock();
    }

    if(count>=POPC_QUEUE_NORMAL) {
        //To many requests: Slowdown the receive thread...
        int step=(count/POPC_QUEUE_NORMAL);
        long t=step*step*step;
        //if (count>POPC_QUEUE_NORMAL+5)
        LOG_WARNING(" Warning: too many requests (unserved requests: %d)",count);
        if(count<=POPC_QUEUE_MAX) {
            popc_usleep(10*t);
        } else {
            while(request_fifo.GetCount()>POPC_QUEUE_MAX) {
                popc_usleep(t*10);
            }
        }
    }
}

bool paroc_broker::OnNewConnection(paroc_connection * /*conn*/) {
    if(obj!=NULL) {
        obj->AddRef();
    }
    return true;
}

/**
 * This method is called when a connection with an interface is closed.
 */
bool paroc_broker::OnCloseConnection(paroc_connection * /*conn*/) {
    if(obj!=NULL) {
        int ret=obj->DecRef();
        if(ret<=0) {
            execCond.broadcast();
        }
    }
    return true;
}


paroc_object * paroc_broker::GetObject() {
    return obj;
}

bool  paroc_broker::ParocCall(paroc_request &req) {
    if(req.methodId[1]>=10) {
        return false;
    }

    unsigned *methodid=req.methodId;
    paroc_buffer *buf=req.data;
    switch(methodid[1]) {
    case 0:
        // BindStatus call
        if(methodid[2] & INVOKE_SYNC) {
            paroc_message_header h(0, 0, INVOKE_SYNC ,"BindStatus");
            buf->Reset();
            buf->SetHeader(h);
            int status = 0;
            POPString enclist;
            paroc_buffer_factory_finder *finder = paroc_buffer_factory_finder::GetInstance();
            int count = finder->GetFactoryCount();
            for(int i = 0; i < count; i++) {
                POPString t;
                if(finder->GetBufferName(i, t)) {
                    enclist += t;
                    if(i < count-1) {
                        enclist += " ";
                    }
                }
            }

            buf->Push("code","int",1);
            buf->Pack(&status,1);
            buf->Pop();

            buf->Push("platform","POPString",1);
            buf->Pack(&paroc_system::platform,1);
            buf->Pop();

            buf->Push("info","POPString",1);
            buf->Pack(&enclist,1);
            buf->Pop();

            buf->Send(req.from);
        }
        break;
    case 1: {
        //AddRef call...
        if(obj==NULL) {
            return false;
        }
        int ret=obj->AddRef();
        if(methodid[2] & INVOKE_SYNC) {
            buf->Reset();
            paroc_message_header h("AddRef");
            buf->SetHeader(h);

            buf->Push("refcount","int",1);
            buf->Pack(&ret,1);
            buf->Pop();

            buf->Send(req.from);
        }
        execCond.broadcast();
    }
    break;
    case 2: {
        // Decrement reference
        if(obj == NULL) {
            return false;
        }
        int ret = obj->DecRef();
        if(methodid[2] & INVOKE_SYNC) {
            buf->Reset();
            paroc_message_header h("DecRef");
            buf->SetHeader(h);

            buf->Push("refcount","int",1);
            buf->Pack(&ret,1);
            buf->Pop();
            buf->Send(req.from);
        }
        execCond.broadcast();
        break;
    }
    case 3: {
        // Negotiate encoding call
        POPString enc;
        buf->Push("encoding","POPString",1);
        buf->UnPack(&enc,1);
        buf->Pop();
        paroc_buffer_factory *fact=paroc_buffer_factory_finder::GetInstance()->FindFactory(enc);
        bool ret;
        if(fact) {
            req.from->SetBufferFactory(fact);
            ret = true;
        } else {
            ret = false;
        }
        if(methodid[2] & INVOKE_SYNC) {
            paroc_message_header h("Encoding");
            buf->SetHeader(h);
            buf->Reset();
            buf->Push("result","bool",1);
            buf->Pack(&ret,1);
            buf->Pop();
            buf->Send(req.from);
        }
        break;
    }
    case 4: {
        // Kill call
        if(obj && obj->CanKill()) {
            LOG_INFO("Object exit by killcall");
            exit(1);
        }
        break;
    }
    case 5: {
        // ObjectAlive call
        if(!obj) {
            return false;
        }
        if(methodid[2] & INVOKE_SYNC) {
            buf->Reset();
            paroc_message_header h("ObjectActive");
            buf->SetHeader(h);
            bool ret=(instanceCount || request_fifo.GetCount());
            buf->Push("result", "bool", 1);
            buf->Pack(&ret, 1);
            buf->Pop();
            buf->Send(req.from);
        }

        break;
    }
#ifdef OD_DISCONNECT
    case 6: {
        //ObjectAlive call
        if(obj==NULL) {
            return false;
        }
        if(methodid[2] & INVOKE_SYNC) {
            buf->Reset();
            paroc_message_header h("ObjectAlive");
            h.SetClassID(0);
            h.SetMethodID(6);
            buf->SetHeader(h);
            buf->Send(req.from);
        }
        break;
    }
#endif
    default:
        return false;
    }
    return true;
}
