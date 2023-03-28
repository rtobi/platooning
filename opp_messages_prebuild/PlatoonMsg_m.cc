//
// Generated file, do not edit! Created by nedtool 5.6 from PlatoonMsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "PlatoonMsg_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(PlatoonMsg)

PlatoonMsg::PlatoonMsg(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->msgType = 0;
    this->txStationID = 0;
    this->platoonID = 0;
    this->innerPosition = 0;
    this->depth = 0;
    this->nonce = 0;
    this->nonceRef = 0;
    this->timestamp = 0;
    this->ivDistance = 0;
    members_arraysize = 0;
    this->members = 0;
}

PlatoonMsg::PlatoonMsg(const PlatoonMsg& other) : ::omnetpp::cPacket(other)
{
    members_arraysize = 0;
    this->members = 0;
    copy(other);
}

PlatoonMsg::~PlatoonMsg()
{
    delete [] this->members;
}

PlatoonMsg& PlatoonMsg::operator=(const PlatoonMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void PlatoonMsg::copy(const PlatoonMsg& other)
{
    this->msgType = other.msgType;
    this->txStationID = other.txStationID;
    this->platoonID = other.platoonID;
    this->innerPosition = other.innerPosition;
    this->depth = other.depth;
    this->nonce = other.nonce;
    this->nonceRef = other.nonceRef;
    this->timestamp = other.timestamp;
    this->ivDistance = other.ivDistance;
    this->edgeID = other.edgeID;
    this->laneID = other.laneID;
    delete [] this->members;
    this->members = (other.members_arraysize==0) ? nullptr : new uint32_t[other.members_arraysize];
    members_arraysize = other.members_arraysize;
    for (unsigned int i=0; i<members_arraysize; i++)
        this->members[i] = other.members[i];
}

void PlatoonMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->txStationID);
    doParsimPacking(b,this->platoonID);
    doParsimPacking(b,this->innerPosition);
    doParsimPacking(b,this->depth);
    doParsimPacking(b,this->nonce);
    doParsimPacking(b,this->nonceRef);
    doParsimPacking(b,this->timestamp);
    doParsimPacking(b,this->ivDistance);
    doParsimPacking(b,this->edgeID);
    doParsimPacking(b,this->laneID);
    b->pack(members_arraysize);
    doParsimArrayPacking(b,this->members,members_arraysize);
}

void PlatoonMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->txStationID);
    doParsimUnpacking(b,this->platoonID);
    doParsimUnpacking(b,this->innerPosition);
    doParsimUnpacking(b,this->depth);
    doParsimUnpacking(b,this->nonce);
    doParsimUnpacking(b,this->nonceRef);
    doParsimUnpacking(b,this->timestamp);
    doParsimUnpacking(b,this->ivDistance);
    doParsimUnpacking(b,this->edgeID);
    doParsimUnpacking(b,this->laneID);
    delete [] this->members;
    b->unpack(members_arraysize);
    if (members_arraysize==0) {
        this->members = 0;
    } else {
        this->members = new uint32_t[members_arraysize];
        doParsimArrayUnpacking(b,this->members,members_arraysize);
    }
}

uint32_t PlatoonMsg::getMsgType() const
{
    return this->msgType;
}

void PlatoonMsg::setMsgType(uint32_t msgType)
{
    this->msgType = msgType;
}

uint32_t PlatoonMsg::getTxStationID() const
{
    return this->txStationID;
}

void PlatoonMsg::setTxStationID(uint32_t txStationID)
{
    this->txStationID = txStationID;
}

uint32_t PlatoonMsg::getPlatoonID() const
{
    return this->platoonID;
}

void PlatoonMsg::setPlatoonID(uint32_t platoonID)
{
    this->platoonID = platoonID;
}

uint32_t PlatoonMsg::getInnerPosition() const
{
    return this->innerPosition;
}

void PlatoonMsg::setInnerPosition(uint32_t innerPosition)
{
    this->innerPosition = innerPosition;
}

uint32_t PlatoonMsg::getDepth() const
{
    return this->depth;
}

void PlatoonMsg::setDepth(uint32_t depth)
{
    this->depth = depth;
}

uint32_t PlatoonMsg::getNonce() const
{
    return this->nonce;
}

void PlatoonMsg::setNonce(uint32_t nonce)
{
    this->nonce = nonce;
}

uint32_t PlatoonMsg::getNonceRef() const
{
    return this->nonceRef;
}

void PlatoonMsg::setNonceRef(uint32_t nonceRef)
{
    this->nonceRef = nonceRef;
}

::omnetpp::simtime_t PlatoonMsg::getTimestamp() const
{
    return this->timestamp;
}

void PlatoonMsg::setTimestamp(::omnetpp::simtime_t timestamp)
{
    this->timestamp = timestamp;
}

uint32_t PlatoonMsg::getIvDistance() const
{
    return this->ivDistance;
}

void PlatoonMsg::setIvDistance(uint32_t ivDistance)
{
    this->ivDistance = ivDistance;
}

const char * PlatoonMsg::getEdgeID() const
{
    return this->edgeID.c_str();
}

void PlatoonMsg::setEdgeID(const char * edgeID)
{
    this->edgeID = edgeID;
}

const char * PlatoonMsg::getLaneID() const
{
    return this->laneID.c_str();
}

void PlatoonMsg::setLaneID(const char * laneID)
{
    this->laneID = laneID;
}

void PlatoonMsg::setMembersArraySize(unsigned int size)
{
    uint32_t *members2 = (size==0) ? nullptr : new uint32_t[size];
    unsigned int sz = members_arraysize < size ? members_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        members2[i] = this->members[i];
    for (unsigned int i=sz; i<size; i++)
        members2[i] = 0;
    members_arraysize = size;
    delete [] this->members;
    this->members = members2;
}

unsigned int PlatoonMsg::getMembersArraySize() const
{
    return members_arraysize;
}

uint32_t PlatoonMsg::getMembers(unsigned int k) const
{
    if (k>=members_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", members_arraysize, k);
    return this->members[k];
}

void PlatoonMsg::setMembers(unsigned int k, uint32_t members)
{
    if (k>=members_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", members_arraysize, k);
    this->members[k] = members;
}

class PlatoonMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    PlatoonMsgDescriptor();
    virtual ~PlatoonMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(PlatoonMsgDescriptor)

PlatoonMsgDescriptor::PlatoonMsgDescriptor() : omnetpp::cClassDescriptor("PlatoonMsg", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

PlatoonMsgDescriptor::~PlatoonMsgDescriptor()
{
    delete[] propertynames;
}

bool PlatoonMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<PlatoonMsg *>(obj)!=nullptr;
}

const char **PlatoonMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *PlatoonMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int PlatoonMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 12+basedesc->getFieldCount() : 12;
}

unsigned int PlatoonMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<12) ? fieldTypeFlags[field] : 0;
}

const char *PlatoonMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "msgType",
        "txStationID",
        "platoonID",
        "innerPosition",
        "depth",
        "nonce",
        "nonceRef",
        "timestamp",
        "ivDistance",
        "edgeID",
        "laneID",
        "members",
    };
    return (field>=0 && field<12) ? fieldNames[field] : nullptr;
}

int PlatoonMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "msgType")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "txStationID")==0) return base+1;
    if (fieldName[0]=='p' && strcmp(fieldName, "platoonID")==0) return base+2;
    if (fieldName[0]=='i' && strcmp(fieldName, "innerPosition")==0) return base+3;
    if (fieldName[0]=='d' && strcmp(fieldName, "depth")==0) return base+4;
    if (fieldName[0]=='n' && strcmp(fieldName, "nonce")==0) return base+5;
    if (fieldName[0]=='n' && strcmp(fieldName, "nonceRef")==0) return base+6;
    if (fieldName[0]=='t' && strcmp(fieldName, "timestamp")==0) return base+7;
    if (fieldName[0]=='i' && strcmp(fieldName, "ivDistance")==0) return base+8;
    if (fieldName[0]=='e' && strcmp(fieldName, "edgeID")==0) return base+9;
    if (fieldName[0]=='l' && strcmp(fieldName, "laneID")==0) return base+10;
    if (fieldName[0]=='m' && strcmp(fieldName, "members")==0) return base+11;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *PlatoonMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint32_t",
        "uint32_t",
        "uint32_t",
        "uint32_t",
        "uint32_t",
        "uint32_t",
        "uint32_t",
        "simtime_t",
        "uint32_t",
        "string",
        "string",
        "uint32_t",
    };
    return (field>=0 && field<12) ? fieldTypeStrings[field] : nullptr;
}

const char **PlatoonMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *PlatoonMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "MessageTypePlatoon";
            return nullptr;
        default: return nullptr;
    }
}

int PlatoonMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    PlatoonMsg *pp = (PlatoonMsg *)object; (void)pp;
    switch (field) {
        case 11: return pp->getMembersArraySize();
        default: return 0;
    }
}

const char *PlatoonMsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    PlatoonMsg *pp = (PlatoonMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string PlatoonMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    PlatoonMsg *pp = (PlatoonMsg *)object; (void)pp;
    switch (field) {
        case 0: return enum2string(pp->getMsgType(), "MessageTypePlatoon");
        case 1: return ulong2string(pp->getTxStationID());
        case 2: return ulong2string(pp->getPlatoonID());
        case 3: return ulong2string(pp->getInnerPosition());
        case 4: return ulong2string(pp->getDepth());
        case 5: return ulong2string(pp->getNonce());
        case 6: return ulong2string(pp->getNonceRef());
        case 7: return simtime2string(pp->getTimestamp());
        case 8: return ulong2string(pp->getIvDistance());
        case 9: return oppstring2string(pp->getEdgeID());
        case 10: return oppstring2string(pp->getLaneID());
        case 11: return ulong2string(pp->getMembers(i));
        default: return "";
    }
}

bool PlatoonMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    PlatoonMsg *pp = (PlatoonMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setMsgType((MessageTypePlatoon)string2enum(value, "MessageTypePlatoon")); return true;
        case 1: pp->setTxStationID(string2ulong(value)); return true;
        case 2: pp->setPlatoonID(string2ulong(value)); return true;
        case 3: pp->setInnerPosition(string2ulong(value)); return true;
        case 4: pp->setDepth(string2ulong(value)); return true;
        case 5: pp->setNonce(string2ulong(value)); return true;
        case 6: pp->setNonceRef(string2ulong(value)); return true;
        case 7: pp->setTimestamp(string2simtime(value)); return true;
        case 8: pp->setIvDistance(string2ulong(value)); return true;
        case 9: pp->setEdgeID((value)); return true;
        case 10: pp->setLaneID((value)); return true;
        case 11: pp->setMembers(i,string2ulong(value)); return true;
        default: return false;
    }
}

const char *PlatoonMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *PlatoonMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    PlatoonMsg *pp = (PlatoonMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


